// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#pragma once

#include "Clang.h"
#include "VAST.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbitfield-enum-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/Type.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Sema/Lookup.h>
#include <clang/Sema/Sema.h>
#include <llvm/ADT/APInt.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/TargetParser/Host.h>
#include <mlir/IR/Block.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/Dialect.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/Region.h>
#include <mlir/IR/TypeSupport.h>
#include <mlir/IR/Types.h>
#pragma GCC diagnostic pop

#include <cassert>
#include <map>
#include <string>
#include <unordered_map>
#include <vast/Dialect/HighLevel/HighLevelOps.hpp>
#include <vast/Dialect/HighLevel/HighLevelTypes.hpp>
#include <vast/Interfaces/TypeQualifiersInterfaces.hpp>
#include <vast/Util/TypeSwitch.hpp>

namespace miller {
namespace ast {

class AST final : public ClangModuleImpl {
 private:
  const bool char_is_unsigned;
  const std::shared_ptr<mlir::Operation> module;
  const mlir::DataLayout dl;

  std::unordered_map<mlir::Operation *, clang::Stmt *> op_to_stmt;
  std::unordered_map<mlir::Operation *, clang::ValueDecl *> op_to_decl;
  std::unordered_map<void *, clang::ValueDecl *> val_to_decl;

  // MLIR types are uniqued, so we can cache any type liftings that we've
  // performed.
  llvm::DenseMap<mlir::Type, clang::QualType> type_map;

  static clang::QualType Qualify(clang::QualType ty,
                                 vast::hl::CVRQualifiersAttr quals);

  static clang::QualType Qualify(clang::QualType ty,
                                 vast::hl::UCVQualifiersAttr quals);

  static clang::QualType Qualify(clang::QualType ty,
                                 vast::hl::CVQualifiersAttr quals);

  bool CharIsUnsigned(void) const;

  // `vast.hl.expr` packages up some computation as an expression. If we see
  // that expression used, then assume that it's actually nested. That is,
  // in code like `(A + (B + C))`, that might, in HL, turn into:
  //
  //        %A = expr {  ...  }
  //        %0 = expr {
  //          %B = expr { ... }
  //          %C = expr { ... }
  //          %1 = hl.add %B, %C
  //          hl.value.yield %1
  //        }
  //        %2 = hl.add %A, %0
  //
  // And so to get back the `(A + (B + C))`, we need to allow that tree to
  // be implicitly build up by *excluding* things like `%B`, `%C`, and `%1`
  // from the `%0 = expr`, so that at the usage site of `%0`, we use the
  // lifted expression.
  static bool ElideFromCompoundStmt(mlir::Operation &op, clang::Stmt *stmt);

 public:
  explicit AST(const llvm::Triple &triple, std::shared_ptr<mlir::Operation> op);

  static std::shared_ptr<AST> CreateFromModule(
      std::shared_ptr<mlir::Operation> op);

  clang::QualType LiftType(mlir::Type ty);
  clang::QualType LiftFunctionType(mlir::FunctionType ty);
  clang::FunctionDecl *LiftFuncOp(clang::DeclContext *sdc,
                                  clang::DeclContext *ldc,
                                  vast::hl::FuncOp func);
  clang::Expr *LiftValue(clang::DeclContext *dc, mlir::Value val);
  clang::Stmt *LiftOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Stmt *LiftOpImpl(clang::DeclContext *dc, mlir::Operation &op);
  clang::DoStmt *LiftDoOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftCondYieldOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftValueYieldOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::ReturnStmt *LiftReturnOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftCStyleCastOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftImplicitCastOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftConstantOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftExprOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftDeclRefOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftLNotOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftNotOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftBinShrOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftBinShlOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftBinAndOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftBinOrOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftBinXorOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftAddIOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftSubIOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftMulIOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftDivUOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftDivSOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftRemUOp(clang::DeclContext *dc, mlir::Operation &op);
  clang::Expr *LiftRemSOp(clang::DeclContext *dc, mlir::Operation &op);
};

}  // namespace ast
}  // namespace miller
