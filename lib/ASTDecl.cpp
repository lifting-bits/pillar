// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "AST.h"
#include "Util.h"

namespace pillar
{
  namespace ast
  {
    static constexpr clang::SourceLocation kEmptyLoc;
    clang::FunctionDecl *AST::LiftFuncOp(clang::DeclContext *sdc,
                                         clang::DeclContext *ldc,
                                         vast::hl::FuncOp func)
    {

      std::string function_name = np.FunctionName(func);

      clang::QualType fty = LiftType(func.getFunctionType());
      llvm::StringRef function_name_str_ref(function_name.c_str(), function_name.length());
      clang::FunctionDecl *func_decl = CreateFunctionDecl(
          sdc, ldc, fty, function_name_str_ref);
      llvm::SmallVector<clang::ParmVarDecl *, 6u> args;

      sdc->addDecl(func_decl);

      // Base case; make sure we can always find this function.
      mlir::Operation *op = reinterpret_cast<mlir::Operation *>(
          const_cast<void *>(func.getAsOpaquePointer()));
      op_to_decl.emplace(
          op,
          func_decl);

      // Lift the arguments.
      unsigned arg_i = 0u;
      for (mlir::BlockArgument arg : func.getArguments())
      {

        // TODO(pag): Figure out how to get this from VAST.
        std::string arg_name_str = np.FunctionParameterName(func, arg_i++);
        clang::IdentifierInfo *arg_name =
            CreateIdentifier(arg_name_str);
        clang::QualType arg_ty = LiftType(arg.getType());
        clang::ParmVarDecl *arg_decl = sema.CheckParameter(
            func_decl, clang::SourceLocation(), clang::SourceLocation{},
            arg_name, arg_ty, ctx.getTrivialTypeSourceInfo(arg_ty),
            clang::SC_None);
        args.push_back(arg_decl);

        // Expose the argument value.
        val_to_decl.emplace(arg.getAsOpaquePointer(), arg_decl);
      }
      func_decl->setParams(args);

      // TODO(pag): Linkage.

      mlir::Region &body = func.getBody();
      if (!body.hasOneBlock())
      {
        assert(body.getBlocks().empty());
        return func_decl;
      }
      auto lift_body = [=, &body, this](void)
      {
        // Lift each statement from the function body, collecting them into
        // `body_stmts`.
        std::vector<clang::Stmt *> body_stmts;
        for (mlir::Operation &op : body.front())
        {

          if (clang::Stmt *stmt = LiftOp(func_decl, op))
          {
            if (!ElideFromCompoundStmt(op, stmt))
            {
              body_stmts.emplace_back(stmt);
            }
          }
        }

        clang::FPOptionsOverride fpo;
        clang::CompoundStmt *body_stmt = clang::CompoundStmt::Create(
            ctx, body_stmts, fpo, kEmptyLoc, kEmptyLoc);
        func_decl->setBody(body_stmt);
      };

      AST::AddToLiftQueue(std::move(lift_body));

      return func_decl;
    }
    clang::VarDecl *AST::LiftVarDeclOp(clang::DeclContext *sdc,
                                       clang::DeclContext *ldc,
                                       vast::hl::VarDeclOp var_decl_op)
    {

      std::string name = np.VariableName(var_decl_op);
      mlir::Type varType = var_decl_op.getType();
      // // Create Clang QualType from MLIR Type
      clang::QualType clang_type = LiftType(varType);
      clang::VarDecl *var_decl = CreateVarDeclFromStrRef(sdc, ldc, clang_type, name);
      sdc->addDecl(var_decl);
      op_to_decl.emplace(
          var_decl_op,
          var_decl);
      if (mlir::Region *init = &(var_decl_op.getInitializer()))
      {

        AddToLiftQueue([=, this]()
                       {
      for (mlir::Block &block : init->getBlocks())
      {
        var_decl->setInit(LiftBlockExpr(sdc, block));
        break;
      } });
      }

      return var_decl;
    }
    clang::FieldDecl *AST::LiftFieldDeclOp(clang::DeclContext *sdc,
                                           clang::DeclContext *ldc, clang::RecordDecl *record,
                                           vast::hl::FieldDeclOp field_decl_op)
    {

      std::string name = np.FieldName(field_decl_op);
      mlir::Type decl_ype = field_decl_op.getType();
      clang::QualType clang_ype = LiftType(decl_ype);
      clang::FieldDecl *field_decl = createFieldDecl(sdc, ldc, record, name, clang_ype);
      return field_decl;
    }
    clang::RecordDecl *AST::LiftStructOp(clang::DeclContext *sdc,
                                         clang::DeclContext *ldc,
                                         vast::hl::StructDeclOp strct_op)
    {
      std::string name = np.StructName(strct_op);
      clang::RecordDecl *record_decl = createRecordDecl(sdc, ldc, name);

      mlir::Region *fields = &(strct_op.getFields());

      for (mlir::Block &block : fields->getBlocks())
      {
        for (mlir::Operation &op : block.getOperations())
        {

          auto field_decl = clang::dyn_cast<clang::FieldDecl>(LiftTagElementOp(record_decl, record_decl, record_decl, op));
          record_decl->addDecl(field_decl);
        }
      }

      sdc->addDecl(record_decl);
      record_decl->completeDefinition();
      op_to_decl.emplace(
          strct_op,
          clang::dyn_cast<ValueDecl>(record_decl));

      return record_decl;
    }
    clang::Decl *AST::LiftTagElementOp(clang::DeclContext *sdc,
                                       clang::DeclContext *ldc, clang::RecordDecl *record_decl,
                                       mlir::Operation &op_)
    {
      return llvm::TypeSwitch<mlir::Operation *, clang::Decl *>(&op_)
          .Case([&](vast::hl::FieldDeclOp field_op)
                { return LiftFieldDeclOp(record_decl, record_decl, record_decl, field_op); })
          .Default([&](mlir::Operation *)
                   { std::cout << "No handler Tag: " << op_.getName().getStringRef().str() << "\n";
                   return nullptr; });

      return nullptr;
    }
    clang::TypedefDecl *AST::LiftTypeDefOp(clang::DeclContext *sdc,
                                           clang::DeclContext *ldc,
                                           vast::hl::TypeDefOp type_def_op)
    {
      std::cout << "TypeDef Lifted\n";
      return nullptr;
    }
  } // namespace ast
} // namespace pillar
