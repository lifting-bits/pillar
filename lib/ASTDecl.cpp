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

    clang::FunctionDecl *AST::LiftFuncOp(clang::DeclContext *sdc,
                                         clang::DeclContext *ldc,
                                         vast::hl::FuncOp func)
    {

      std::string functionName = np.FunctionName(func);

      clang::QualType fty = LiftType(func.getFunctionType());
      llvm::StringRef functionNameStrRef(functionName.c_str(), functionName.length());
      clang::FunctionDecl *func_decl = CreateFunctionDecl(
          sdc, ldc, fty, functionNameStrRef);
      llvm::SmallVector<clang::ParmVarDecl *, 6u> args;

      ldc->addDecl(func_decl);

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
        std::string argNameStr = np.FunctionParameterName(func, arg_i++);
        clang::IdentifierInfo *arg_name =
            CreateIdentifier(argNameStr);
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
      AST::lift_queue->emplace_back([&body, func_decl, functionName, this](void)
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
                                          ctx, body_stmts, fpo, clang::SourceLocation(), clang::SourceLocation());
                                      func_decl->setBody(body_stmt);
                                      // std::cout << "func " << functionName << " body lifted\n";
                                    });

      return func_decl;
    }
    void AST::LiftVarDeclOp(clang::DeclContext *sdc,
                            clang::DeclContext *ldc,
                            vast::hl::VarDeclOp varDecl)
    {
      std::cout << "VarDecl Lifted\n";
    }
    void AST::LiftTypeDefOp(clang::DeclContext *sdc,
                            clang::DeclContext *ldc,
                            vast::hl::TypeDefOp typeDef)
    {
      std::cout << "TypeDef Lifted\n";
    }
  } // namespace ast
} // namespace pillar
