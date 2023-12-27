// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "AST.h"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/PrettyPrinter.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Sema/Lookup.h>
#include <clang/Sema/Sema.h>
#include <vast/Util/TypeSwitch.hpp>
#include <llvm/ADT/TypeSwitch.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbitfield-enum-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic pop

namespace pillar
{
  namespace ast
  {
    std::vector<std::function<void(void)>> *AST::lift_queue = new std::vector<std::function<void(void)>>();

    AST::AST(const llvm::Triple &triple, std::shared_ptr<mlir::Operation> op_)
        : ClangModuleImpl(triple),
          char_is_unsigned(CharIsUnsigned()),
          module(std::move(op_)),
          dl(mlir::dyn_cast<mlir::ModuleOp>(module.get())) {}

    bool AST::ElideFromCompoundStmt(mlir::Operation &op, clang::Stmt *stmt)
    {
      if (op.use_empty())
      {
        return false;
      }
      else if (mlir::isa<vast::hl::ExprOp>(op))
      {
        return true;
      }
      else
      {
        return clang::isa<clang::Expr>(stmt);
      }
    }

    std::shared_ptr<AST> AST::CreateFromModule(
        std::shared_ptr<mlir::Operation> op)
    {
      mlir::Operation *currentOp = op.get();
      auto moduleOp = dyn_cast<mlir::ModuleOp>(currentOp);
      auto triple_attr = moduleOp->getAttrOfType<mlir::StringAttr>("vast.core.target_triple");

      llvm::Triple triple;
      if (triple_attr)
      {
        triple = llvm::Triple(triple_attr.getValue().str());
      }
      std::shared_ptr<AST> ast = std::make_shared<ast::AST>(triple, std::move(op));
      clang::TranslationUnitDecl *tu = ast->ctx.getTranslationUnitDecl();

      ///////
      for (mlir::Operation &op : moduleOp.getBody()->getOperations())
      {
        // Print the operation name.
        llvm::TypeSwitch<mlir::Operation *>(&op)
            .Case([&](vast::hl::FuncOp)
                  { (void)ast->LiftFuncOp(tu, tu, mlir::dyn_cast<vast::hl::FuncOp>(std::move(op))); })
            .Case([&](vast::hl::VarDeclOp)
                  { (void)ast->LiftVarDeclOp(tu, tu, mlir::dyn_cast<vast::hl::VarDeclOp>(std::move(op))); })
            .Case([&](vast::hl::TypeDefOp)
                  { (void)ast->LiftTypeDefOp(tu, tu, mlir::dyn_cast<vast::hl::TypeDefOp>(std::move(op))); })
            .Case([&](vast::hl::EnumDeclOp) {})
            .Case([&](vast::hl::CxxStructDeclOp) {})
            .Case([&](vast::hl::ClassDeclOp) {})
            .Default([&](mlir::Operation *)
                     { std::cout << "No handler for: " << op.getName().getStringRef().str() << "\n"; });
      }

      for (size_t i = 0; i < lift_queue->size(); i++)
      {
        (*lift_queue)[i]();
      }

      tu->dumpColor();
      tu->print(llvm::errs());
      return ast;
    }

    clang::Stmt *AST::LiftOp(clang::DeclContext *dc, mlir::Operation &op)
    {
      if (auto it = op_to_stmt.find(&op); it != op_to_stmt.end())
      {
        return it->second;
      }

      clang::Stmt *ret = LiftOpImpl(dc, op);
      if (!ret)
      {
        op.dump();
        assert(false);
        return nullptr;
      }

      op_to_stmt.emplace(&op, ret);
      return ret;
    }

    clang::Expr *AST::LiftValue(clang::DeclContext *dc, mlir::Value val)
    {
      if (mlir::Operation *op = val.getDefiningOp())
      {
        if (auto decl_it = op_to_decl.find(op); decl_it != op_to_decl.end())
        {
          return CreateDeclRef(decl_it->second);
        }

        if (auto stmt_it = op_to_stmt.find(op); stmt_it != op_to_stmt.end())
        {
          return clang::dyn_cast<clang::Expr>(stmt_it->second);
        }
      }

      void *opaque = val.getAsOpaquePointer();
      if (auto decl_it = val_to_decl.find(opaque); decl_it != val_to_decl.end())
      {
        return CreateDeclRef(decl_it->second);
      }

      val.dump();
      assert(false);
      return nullptr;
    }

  } // namespace ast
} // namespace pillar
