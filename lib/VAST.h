// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#pragma once

#include <miller/VAST.h>

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>

#include <optional>

#define HL_DIALECT_OPS(m) \
    m(AccessSpecifierOp) \
    m(AddFAssignOp) \
    m(AddFOp) \
    m(AddIAssignOp) \
    m(AddIOp) \
    m(AddrLabelExpr) \
    m(AddressOf) \
    m(AlignOfExprOp) \
    m(AlignOfTypeOp) \
    m(AsmOp) \
    m(AssignOp) \
    m(BinAShrAssignOp) \
    m(BinAShrOp) \
    m(BinAndAssignOp) \
    m(BinAndOp) \
    m(BinComma) \
    m(BinLAndOp) \
    m(BinLOrOp) \
    m(BinLShrAssignOp) \
    m(BinLShrOp) \
    m(BinOrAssignOp) \
    m(BinOrOp) \
    m(BinShlAssignOp) \
    m(BinShlOp) \
    m(BinXorAssignOp) \
    m(BinXorOp) \
    m(BuiltinBitCastOp) \
    m(CStyleCastOp) \
    m(CallOp) \
    m(ClassDeclOp) \
    m(CmpOp) \
    m(ConstantOp) \
    m(CxxBaseSpecifierOp) \
    m(CxxStructDeclOp) \
    m(DeclRefOp) \
    m(Deref) \
    m(DivFAssignOp) \
    m(DivFOp) \
    m(DivSAssignOp) \
    m(DivSOp) \
    m(DivUAssignOp) \
    m(DivUOp) \
    m(EnumConstantOp) \
    m(EnumDeclOp) \
    m(EnumRefOp) \
    m(ExprOp) \
    m(ExtensionOp) \
    m(FCmpOp) \
    m(FieldDeclOp) \
    m(FuncRefOp) \
    m(GlobalRefOp) \
    m(BreakOp) \
    m(CaseOp) \
    m(CondOp) \
    m(CondYieldOp) \
    m(ContinueOp) \
    m(DefaultOp) \
    m(DoOp) \
    m(EmptyDeclOp) \
    m(ForOp) \
    m(FuncOp) \
    m(GotoStmt) \
    m(IfOp) \
    m(LabelDeclOp) \
    m(LabelStmt) \
    m(SkipStmt) \
    m(SwitchOp) \
    m(TypeYieldOp) \
    m(ValueYieldOp) \
    m(VarDeclOp) \
    m(WhileOp) \
    m(ImplicitCastOp) \
    m(IndirectCallOp) \
    m(InitListExpr) \
    m(LNotOp) \
    m(MinusOp) \
    m(MulFAssignOp) \
    m(MulFOp) \
    m(MulIAssignOp) \
    m(MulIOp) \
    m(NotOp) \
    m(PlusOp) \
    m(PostDecOp) \
    m(PostIncOp) \
    m(PreDecOp) \
    m(PreIncOp) \
    m(PredefinedExpr) \
    m(RecordMemberOp) \
    m(RemFAssignOp) \
    m(RemFOp) \
    m(RemSAssignOp) \
    m(RemSOp) \
    m(RemUAssignOp) \
    m(RemUOp) \
    m(ReturnOp) \
    m(SizeOfExprOp) \
    m(SizeOfTypeOp) \
    m(StmtExprOp) \
    m(StructDeclOp) \
    m(SubFAssignOp) \
    m(SubFOp) \
    m(SubIAssignOp) \
    m(SubIOp) \
    m(SubscriptOp) \
    m(ThisOp) \
    m(TranslationUnitOp) \
    m(TypeDeclOp) \
    m(TypeDefOp) \
    m(TypeOfExprOp) \
    m(TypeOfTypeOp) \
    m(UnionDeclOp) \
    m(UnreachableOp)

namespace mlir {
class DialectRegistry;
}  // namespace mlir
namespace vast {

#define DECLARE_HL_OP(n) class n;
HL_DIALECT_OPS(DECLARE_HL_OP)
#undef DECLARE_HL_OP

}  // namespace vast
namespace miller {

enum class HlOpKind {
#define DECLARE_HL_OP_TYPE(n) k ## n,
HL_DIALECT_OPS(DECLARE_HL_OP_TYPE)
#undef DECLARE_HL_OP_TYPE
  kUnknown
};

HlOpKind KindOf(mlir::Operation *);
HlOpKind KindOf(mlir::Operation &);

class VASTModuleImpl final {
 public:

  mlir::MLIRContext context;
  mlir::OwningOpRef<mlir::ModuleOp> module;

  ~VASTModuleImpl(void);
  VASTModuleImpl(void);
};

}  // namespace miller
