// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#pragma once

#include <pillar/Clang.h>
#include <clang/AST/OperationKinds.h>

namespace llvm {
class StringRef;
class Triple;
}  // namespace llvm
namespace clang {
class ASTContext;
class ASTUnit;
class BinaryOperator;
class DeclContext;
class DeclRefExpr;
class DoStmt;
class Expr;
class FunctionDecl;
class IdentifierInfo;
class ParenExpr;
class QualType;
class ReturnStmt;
class Sema;
class Stmt;
class UnaryOperator;
class ValueDecl;
}  // namespace clang
namespace vast {

}  // namespace vast
namespace pillar {

class ClangModuleImpl {

  // Force us to initialize LLVM.
  const void * const llvm;

  // An empty ASTUnit initialized from parsing empty code.
  std::unique_ptr<clang::ASTUnit> unit;

  ClangModuleImpl(void) = delete;

 public:
  clang::ASTContext &ctx;
  clang::Sema &sema;

  virtual ~ClangModuleImpl(void);
  explicit ClangModuleImpl(const llvm::Triple &triple);

  clang::IdentifierInfo *CreateIdentifier(const llvm::StringRef &name);

  inline clang::FunctionDecl *CreateFunctionDecl(
      clang::DeclContext *sdc, clang::DeclContext *ldc,
      const clang::QualType &type, const llvm::StringRef &name) {
    return CreateFunctionDecl(sdc, ldc, type, CreateIdentifier(name));
  }

  clang::FunctionDecl *CreateFunctionDecl(
      clang::DeclContext *sdc, clang::DeclContext *ldc,
      const clang::QualType &type, clang::IdentifierInfo *id);

  clang::DoStmt *CreateDo(clang::Expr *cond, clang::Stmt *body);
  clang::DeclRefExpr *CreateDeclRef(clang::ValueDecl *val);
  clang::ParenExpr *CreateParen(clang::Expr *expr);

  // Unary operators
  clang::UnaryOperator *CreateUnaryOp(clang::UnaryOperatorKind opc,
                                      clang::Expr *expr);

  clang::UnaryOperator *CreateDeref(clang::Expr *expr) {
    return CreateUnaryOp(clang::UO_Deref, expr);
  }

  clang::UnaryOperator *CreateAddrOf(clang::Expr *expr) {
    return CreateUnaryOp(clang::UO_AddrOf, expr);
  }

  clang::UnaryOperator *CreateLNot(clang::Expr *expr) {
    return CreateUnaryOp(clang::UO_LNot, expr);
  }

  clang::UnaryOperator *CreateNot(clang::Expr *expr) {
    return CreateUnaryOp(clang::UO_Not, expr);
  }

  // Binary operators
  clang::BinaryOperator *CreateBinaryOp(clang::BinaryOperatorKind opc,
                                        clang::Expr *lhs, clang::Expr *rhs);
  // Logical binary operators
  clang::BinaryOperator *CreateLAnd(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_LAnd, lhs, rhs);
  }

  clang::BinaryOperator *CreateLOr(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_LOr, lhs, rhs);
  }

  // Comparison operators
  clang::BinaryOperator *CreateEQ(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_EQ, lhs, rhs);
  }

  clang::BinaryOperator *CreateNE(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_NE, lhs, rhs);
  }

  clang::BinaryOperator *CreateGE(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_GE, lhs, rhs);
  }

  clang::BinaryOperator *CreateGT(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_GT, lhs, rhs);
  }

  clang::BinaryOperator *CreateLE(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_LE, lhs, rhs);
  }

  clang::BinaryOperator *CreateLT(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_LT, lhs, rhs);
  }

  // Bitwise binary operators
  clang::BinaryOperator *CreateAnd(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_And, lhs, rhs);
  }

  clang::BinaryOperator *CreateOr(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Or, lhs, rhs);
  }

  clang::BinaryOperator *CreateXor(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Xor, lhs, rhs);
  }

  clang::BinaryOperator *CreateShl(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Shl, lhs, rhs);
  }

  clang::BinaryOperator *CreateShr(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Shr, lhs, rhs);
  }

  // Arithmetic operators
  clang::BinaryOperator *CreateAdd(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Add, lhs, rhs);
  }

  clang::BinaryOperator *CreateSub(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Sub, lhs, rhs);
  }

  clang::BinaryOperator *CreateMul(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Mul, lhs, rhs);
  }

  clang::BinaryOperator *CreateDiv(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Div, lhs, rhs);
  }

  clang::BinaryOperator *CreateRem(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Rem, lhs, rhs);
  }

  clang::BinaryOperator *CreateAssign(clang::Expr *lhs, clang::Expr *rhs) {
    return CreateBinaryOp(clang::BO_Assign, lhs, rhs);
  }

  clang::ReturnStmt *CreateReturn(clang::Expr *val);
};

}  // namespace pillar
