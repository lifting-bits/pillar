// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "AST.h"

namespace miller {
namespace ast {

clang::FunctionDecl *AST::LiftFuncOp(clang::DeclContext *sdc,
                                     clang::DeclContext *ldc,
                                     vast::hl::FuncOp func) {
  clang::QualType fty = LiftType(func.getFunctionType());
  clang::FunctionDecl *func_decl = CreateFunctionDecl(
      sdc, ldc, fty, func.getName());
  llvm::SmallVector<clang::ParmVarDecl *, 6u> args;

  ldc->addDecl(func_decl);

  // Base case; make sure we can always find this function.
  op_to_decl.emplace(
      reinterpret_cast<mlir::Operation *>(
          const_cast<void *>(func.getAsOpaquePointer())),
      func_decl);

  // Lift the arguments.
  unsigned arg_i = 0u;
  for (mlir::BlockArgument arg : func.getArguments()) {

    // TODO(pag): Figure out how to get this from VAST.
    clang::IdentifierInfo *arg_name =
        CreateIdentifier("arg_" + std::to_string(arg_i++));
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
  if (!body.hasOneBlock()) {
    assert(body.getBlocks().empty());
    return func_decl;
  }

  // Lift each statement from the function body, collecting them into
  // `body_stmts`.
  std::vector<clang::Stmt *> body_stmts;
  for (mlir::Operation &op : body.front()) {
    if (clang::Stmt *stmt = LiftOp(func_decl, op)) {
      if (!ElideFromCompoundStmt(op, stmt)) {
        body_stmts.emplace_back(stmt);
      }
    }
  }

  clang::FPOptionsOverride fpo;
  clang::CompoundStmt *body_stmt = clang::CompoundStmt::Create(
      ctx, body_stmts, fpo, clang::SourceLocation(), clang::SourceLocation());
  func_decl->setBody(body_stmt);
  return func_decl;
}

}  // namespace ast
}  // namespace miller

