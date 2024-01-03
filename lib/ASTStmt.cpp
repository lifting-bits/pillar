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
    namespace
    {
      static const clang::FPOptionsOverride kEmptyFPO;
      static constexpr clang::SourceLocation kEmptyLoc;

      static clang::CastKind ConvertCastKind(vast::hl::CastKind vck)
      {
#define MATCH_CK(ck)           \
  case vast::hl::CastKind::ck: \
    return clang::CK_##ck;
        switch (vck)
        {
          MATCH_CK(NoOp)
          MATCH_CK(IntegralCast)
          MATCH_CK(FloatingCast)
          MATCH_CK(IntegralToFloating)
          MATCH_CK(IntegralToBoolean)
          MATCH_CK(FloatingToFixedPoint)
          MATCH_CK(FloatingToIntegral)
          MATCH_CK(FloatingToBoolean)
          MATCH_CK(FixedPointToFloating)
          MATCH_CK(FixedPointToIntegral)
          MATCH_CK(FixedPointToBoolean)
          MATCH_CK(LValueToRValue)
          MATCH_CK(LValueToRValueBitCast)
          MATCH_CK(PointerToIntegral)
          MATCH_CK(PointerToBoolean)
        default:
          assert(false);
          return clang::CK_BitCast;
        }
#undef MATCH_CK
      }

    } // namespace

    clang::Expr *AST::LiftAddIOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::AddIOp op = mlir::dyn_cast<vast::hl::AddIOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateAdd(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftSubIOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::SubIOp op = mlir::dyn_cast<vast::hl::SubIOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateSub(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftMulIOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::MulIOp op = mlir::dyn_cast<vast::hl::MulIOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateMul(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftDivSOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::DivSOp op = mlir::dyn_cast<vast::hl::DivSOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateDiv(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftDivUOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::DivUOp op = mlir::dyn_cast<vast::hl::DivUOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateDiv(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftRemSOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::RemSOp op = mlir::dyn_cast<vast::hl::RemSOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateRem(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftRemUOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::RemUOp op = mlir::dyn_cast<vast::hl::RemUOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateRem(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftAShrOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinAShrOp op = mlir::dyn_cast<vast::hl::BinAShrOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateShr(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftLShrOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinLShrOp op = mlir::dyn_cast<vast::hl::BinLShrOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateShr(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftShlOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinShlOp op = mlir::dyn_cast<vast::hl::BinShlOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateShl(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftBinAndOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinAndOp op = mlir::dyn_cast<vast::hl::BinAndOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateAnd(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftBinOrOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinOrOp op = mlir::dyn_cast<vast::hl::BinOrOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateOr(lhs_expr, rhs_expr);
    }

    clang::Expr *AST::LiftBinXorOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::BinXorOp op = mlir::dyn_cast<vast::hl::BinXorOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      assert(lhs_expr != nullptr);
      assert(rhs_expr != nullptr);
      return CreateXor(lhs_expr, rhs_expr);
    }

    // Lift a bitwise negation, e.g. `operator~` in C.
    //
    //      %13 = hl.not %12 : !hl.int
    clang::Expr *AST::LiftNotOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::NotOp op = mlir::dyn_cast<vast::hl::NotOp>(op_);
      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateNot(sub_expr);
    }

    // Lift a logical negation, e.g. `operator!` in C.
    //
    //      %13 = hl.lnot %12 : !hl.int
    clang::Expr *AST::LiftLNotOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::LNotOp op = mlir::dyn_cast<vast::hl::LNotOp>(op_);
      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateLNot(sub_expr);
    }

    // Lift a reference to a prior declaration. E.g.
    //
    //      %3 = hl.ref %arg0 : !hl.lvalue<!hl.long>
    clang::Expr *AST::LiftDeclRefOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::DeclRefOp op = mlir::dyn_cast<vast::hl::DeclRefOp>(op_);
      return LiftValue(dc, op.getDecl());
    }

    // Encapsulates some kind of multi-step expresssion, which yields out a result.
    // Thus, we want to lift the set of things inside of it, and the value we return
    // is whatever is yielded.
    //
    //      %9 = hl.expr : !hl.int< unsigned > {
    //        ...
    //        hl.value.yield %15 : !hl.int< unsigned >
    //      }
    clang::Expr *AST::LiftExprOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::ExprOp op = mlir::dyn_cast<vast::hl::ExprOp>(op_);
      clang::Expr *result_expr = nullptr;
      for (mlir::Operation &sub_op : op.getSubexpr().getOps())
      {
        if (clang::Stmt *sub_expr = LiftOp(dc, sub_op))
        {
          if (mlir::isa<vast::hl::ValueYieldOp>(sub_op))
          {
            assert(!result_expr);
            result_expr = clang::dyn_cast<clang::Expr>(sub_expr);
          }
        }
      }
      assert(result_expr != nullptr);
      return result_expr;
    }
    clang::CompoundStmt *AST::LiftRegion(clang::DeclContext *dc, mlir::Region &region)
    {
      std::vector<clang::Stmt *> body_stmts;
      for (mlir::Operation &body_op : region.getOps())
      {
        if (clang::Stmt *body_stmt = LiftOp(dc, body_op))
        {
          if (!ElideFromCompoundStmt(body_op, body_stmt))
          {
            body_stmts.push_back(body_stmt);
          }
        }
      }

      return CreateCompoundStmt(body_stmts);
    }
    clang::Expr *AST::LiftBlockExpr(clang::DeclContext *dc, mlir::Block &block)
    {
      clang::Expr *result_expr = nullptr;
      for (mlir::Operation &op : block.getOperations())
      {
        if (clang::Stmt *sub_expr = LiftOp(dc, op))
        {
          if (mlir::isa<vast::hl::ValueYieldOp>(op) || mlir::isa<vast::hl::CondYieldOp>(op) || mlir::isa<vast::hl::PostIncOp>(op))
          {
            assert(!result_expr);
            result_expr = clang::dyn_cast<clang::Expr>(sub_expr);
          }
        }
      }
      assert(result_expr != nullptr);
      return result_expr;
    }

    // Lift a literal constant. The value is stored in an attribute. E.g.:
    //
    //      %12 = hl.const #hl.integer<0> : !hl.int
    //
    // TODO(pag): Character literals.
    // TODO(pag): Exactness of floating point literals.
    clang::Expr *AST::LiftConstantOp(clang::DeclContext *dc,
                                     mlir::Operation &op_)
    {
      vast::hl::ConstantOp op = mlir::dyn_cast<vast::hl::ConstantOp>(op_);

      return vast::TypeSwitch<mlir::TypedAttr, clang::Expr *>(op.getValue())
          .Case<vast::core::BooleanAttr>(
              [=, this](vast::core::BooleanAttr v) -> clang::Expr *
              {
                (void)this;
                v.dump();
                assert(false);
                return nullptr;
              })
          .Case<vast::core::IntegerAttr>(
              [=, this](vast::core::IntegerAttr v) -> clang::Expr *
              {
                (void)this;
                mlir::Type v_type = v.getType();
                llvm::APSInt v_val = v.getValue();
                llvm::ArrayRef<uint64_t> data(v_val.getRawData(),
                                              v_val.getNumWords());
                llvm::APInt val(dl.getTypeSizeInBits(v_type), data);
                return clang::IntegerLiteral::Create(
                    ctx, val, LiftType(v_type), kEmptyLoc);
              })
          .Case<vast::core::FloatAttr>(
              [=, this](vast::core::FloatAttr v) -> clang::Expr *
              {
                (void)this;
                return clang::FloatingLiteral::Create(
                    ctx, v.getValue(), /* IsExact= */ true, LiftType(v.getType()),
                    kEmptyLoc);
              })
          .Case<mlir::StringAttr>(
              [=, this](mlir::StringAttr v) -> clang::Expr *
              {
                (void)this;
                auto val = v.getValue();
                auto type{ctx.getStringLiteralArrayType(ctx.CharTy, val.size())};
                return clang::StringLiteral::Create(ctx, val, clang::StringLiteral::StringKind::Ordinary, false, type, kEmptyLoc);
              })
          .Default(
              [=, this](mlir::TypedAttr v) -> clang::Expr *
              {
                (void)this;
                v.dump();
                assert(false);
                return nullptr;
              });
    }

    // Yield the resulting value from the end of an expression.
    //
    //      %9 = hl.expr : !hl.int< unsigned > {
    //        ...
    //        hl.value.yield %15 : !hl.int< unsigned >
    //      }
    clang::Expr *AST::LiftValueYieldOp(clang::DeclContext *dc,
                                       mlir::Operation &op_)
    {
      vast::hl::ValueYieldOp op = mlir::dyn_cast<vast::hl::ValueYieldOp>(op_);
      return LiftValue(dc, op.getResult());
    }

    // Signal which value is "yielded" out to some conditional operator, such as
    // `cond` in a `do { ... } while (cond);` statement. This is basically a pass
    // through for us.
    //
    //      hl.do {
    //        ...
    //      } while {
    //        ...
    //        hl.cond.yield %4 : !hl.bool
    //      }
    clang::Expr *AST::LiftCondYieldOp(clang::DeclContext *dc,
                                      mlir::Operation &op_)
    {
      vast::hl::CondYieldOp op = mlir::dyn_cast<vast::hl::CondYieldOp>(op_);
      return LiftValue(dc, op.getResult());
    }

    // Lift a return value. For example:
    //
    //      hl.return %2 : !hl.int
    clang::ReturnStmt *AST::LiftReturnOp(clang::DeclContext *dc,
                                         mlir::Operation &op_)
    {
      vast::hl::ReturnOp op = mlir::dyn_cast<vast::hl::ReturnOp>(op_);
      clang::Expr *ret_expr = nullptr;
      for (mlir::Value ret_val : op.getResult())
      {
        assert(!ret_expr);
        ret_expr = clang::dyn_cast<clang::Expr>(LiftValue(dc, ret_val));
      }
      return CreateReturn(ret_expr);
    }

    // Lift an explicit C-style cast op, e.g. `(int) foo`.
    //
    //      %7 = hl.cstyle_cast %6 NoOp : !hl.long -> !hl.long
    clang::Expr *AST::LiftCStyleCastOp(clang::DeclContext *dc,
                                       mlir::Operation &op_)
    {
      vast::hl::CStyleCastOp op = mlir::dyn_cast<vast::hl::CStyleCastOp>(op_);
      mlir::Type op_type = op.getType();
      mlir::Value casted_val = op.getValue();
      clang::Expr *casted_expr = LiftValue(dc, casted_val);
      clang::QualType dest_type = LiftType(op_type);
      clang::ExprValueKind evk = op_type.isa<vast::hl::LValueType>() ? clang::ExprValueKind::VK_LValue : clang::ExprValueKind::VK_PRValue;

      return clang::CStyleCastExpr::Create(
          ctx, dest_type, evk, ConvertCastKind(op.getKind()), casted_expr,
          /* BasePath= */ nullptr, kEmptyFPO,
          ctx.getTrivialTypeSourceInfo(dest_type),
          kEmptyLoc, kEmptyLoc);
    }

    // Lift an implicit cast. This basically directly corresponds to an implicit
    // cast derived from the Clang AST. It has zero regions, and operates on a
    // value, generating a value as its result. For example:
    //
    //      %14 = hl.implicit_cast %13 IntegralCast : !hl.int -> !hl.int< unsigned >
    clang::Expr *AST::LiftImplicitCastOp(clang::DeclContext *dc,
                                         mlir::Operation &op_)
    {
      vast::hl::ImplicitCastOp op = mlir::dyn_cast<vast::hl::ImplicitCastOp>(op_);
      mlir::Type op_type = op.getType();

      clang::ExprValueKind evk = op_type.isa<vast::hl::LValueType>() ? clang::ExprValueKind::VK_LValue : clang::ExprValueKind::VK_PRValue;

      mlir::Value casted_val = op.getValue();
      clang::QualType dest_type = LiftType(op_type);
      clang::Expr *casted_expr = LiftValue(dc, casted_val);

      return clang::ImplicitCastExpr::Create(
          ctx, dest_type, ConvertCastKind(op.getKind()), casted_expr,
          /* BasePath= */ nullptr, evk, kEmptyFPO);
    }

    // Lift a do operation into a `do { .... } while (...);` statement.
    clang::DoStmt *AST::LiftDoOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::DoOp op = mlir::dyn_cast<vast::hl::DoOp>(op_);

      // Lift each statement in the body of this `do`.
      llvm::SmallVector<clang::Stmt *, 8> body_stmts;
      for (mlir::Operation &body_op : op.getBodyRegion().getOps())
      {
        if (clang::Stmt *body_stmt = LiftOp(dc, body_op))
        {
          if (!ElideFromCompoundStmt(body_op, body_stmt))
          {
            body_stmts.push_back(body_stmt);
          }
        }
      }

      clang::CompoundStmt *body = clang::CompoundStmt::Create(
          ctx, body_stmts, kEmptyFPO, kEmptyLoc, kEmptyLoc);

      // Lift each statement in the condition.
      clang::Expr *cond_expr = nullptr;
      for (mlir::Operation &cond_op : op.getCondRegion().getOps())
      {
        if (clang::Stmt *stmt = LiftOp(dc, cond_op))
        {
          if (mlir::isa<vast::hl::CondYieldOp>(cond_op))
          {
            assert(!cond_expr);
            cond_expr = clang::dyn_cast<clang::Expr>(stmt);
          }
        }
      }

      return CreateDo(cond_expr, body);
    }

    clang::DeclStmt *AST::LiftVarDeclOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      auto var_decl = LiftVarDeclOp(dc, dc, mlir::dyn_cast<vast::hl::VarDeclOp>(op_));
      return new (ctx) clang::DeclStmt(clang::DeclGroupRef(var_decl), kEmptyLoc,
                                       kEmptyLoc);
    }

    clang::Expr *AST::LiftInitListExpr(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::InitListExpr op = mlir::dyn_cast<vast::hl::InitListExpr>(op_);

      std::vector<clang::Expr *> expressions;

      for (mlir::Value operand : op.getElements())
      {
        expressions.push_back(LiftValue(dc, operand));
      }

      return new (ctx) clang::InitListExpr(ctx, kEmptyLoc, expressions, kEmptyLoc);
    }

    clang::Stmt *AST::LiftAssignOp(clang::DeclContext *dc,
                                   mlir::Operation &op_)
    {
      vast::hl::AssignOp op = mlir::dyn_cast<vast::hl::AssignOp>(op_);

      clang::Expr *src_expr = LiftValue(dc, op.getSrc());
      clang::Expr *dst_expr = LiftValue(dc, op.getDst());

      assert(dst_expr != nullptr);
      assert(src_expr != nullptr);
      return CreateAssign(dst_expr, src_expr);
    }
    clang::IfStmt *AST::LiftIfOp(clang::DeclContext *dc, mlir::Operation &op_)
    {

      vast::hl::IfOp op = mlir::dyn_cast<vast::hl::IfOp>(op_);
      bool has_else = op.hasElse();
      clang::Expr *cond_expr = nullptr;
      for (mlir::Block &block : op.getCondRegion().getBlocks())
      {
        cond_expr = LiftBlockExpr(dc, block);
        break;
      }

      clang::CompoundStmt *then_stmt = LiftRegion(dc, op.getThenRegion());
      clang::CompoundStmt *else_stmt = LiftRegion(dc, op.getElseRegion());

      assert(cond_expr != nullptr);
      assert(then_stmt != nullptr);

      return CreateIf(cond_expr, then_stmt, has_else, else_stmt);
    }
    clang::WhileStmt *AST::LiftWhileOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::WhileOp op = mlir::dyn_cast<vast::hl::WhileOp>(op_);
      clang::Expr *cond_expr = nullptr;
      for (mlir::Block &block : op.getCondRegion().getBlocks())
      {
        cond_expr = LiftBlockExpr(dc, block);
        break;
      }
      clang::CompoundStmt *body = LiftRegion(dc, op.getBodyRegion());

      return CreateWhile(cond_expr, body);
    }
    clang::CompoundStmt *AST::LiftScopeOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::core::ScopeOp op = mlir::dyn_cast<vast::core::ScopeOp>(op_);
      auto scope_stmt = LiftRegion(dc, op.getBody());
      return scope_stmt;
    }
    clang::Expr *AST::LiftCmpOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::CmpOp op = mlir::dyn_cast<vast::hl::CmpOp>(op_);
      clang::Expr *lhs_expr = LiftValue(dc, op.getLhs());
      clang::Expr *rhs_expr = LiftValue(dc, op.getRhs());
      switch (op.getPredicate())
      {
      case vast::hl::Predicate::eq:
        return CreateEQ(lhs_expr, rhs_expr);
      case vast::hl::Predicate::ne:
        return CreateNE(lhs_expr, rhs_expr);
      case vast::hl::Predicate::slt:
      case vast::hl::Predicate::ult:
        return CreateLT(lhs_expr, rhs_expr);
      case vast::hl::Predicate::sgt:
      case vast::hl::Predicate::ugt:
        return CreateGT(lhs_expr, rhs_expr);
      case vast::hl::Predicate::sle:
      case vast::hl::Predicate::ule:
        return CreateLE(lhs_expr, rhs_expr);
      case vast::hl::Predicate::sge:
      case vast::hl::Predicate::uge:
        return CreateGE(lhs_expr, rhs_expr);
      default:
        throw std::invalid_argument("Unsupported comparison operation");
      }
      return nullptr;
    }
    clang::ForStmt *AST::LiftForOp(clang::DeclContext *dc, mlir::Operation &op_)
    {

      vast::hl::ForOp op = mlir::dyn_cast<vast::hl::ForOp>(op_);
      clang::Expr *cond_expr = nullptr;
      for (mlir::Block &block : op.getCondRegion().getBlocks())
      {
        cond_expr = LiftBlockExpr(dc, block);
        break;
      }

      clang::Expr *inc = nullptr;
      for (mlir::Block &block : op.getIncrRegion())
      {

        inc = LiftBlockExpr(dc, block);
      }
      clang::CompoundStmt *body = LiftRegion(dc, op.getBodyRegion());

      return CreateFor(nullptr, cond_expr, inc, body);
    }
    clang::UnaryOperator *AST::LiftPostIncOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::PostIncOp op = mlir::dyn_cast<vast::hl::PostIncOp>(op_);

      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateUnaryOp(clang::UO_PostInc, sub_expr);
    }
    clang::UnaryOperator *AST::LiftPostDecOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::PostDecOp op = mlir::dyn_cast<vast::hl::PostDecOp>(op_);

      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateUnaryOp(clang::UO_PostDec, sub_expr);
    }
    clang::UnaryOperator *AST::LiftPreIncOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::PreIncOp op = mlir::dyn_cast<vast::hl::PreIncOp>(op_);

      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateUnaryOp(clang::UO_PreInc, sub_expr);
    }
    clang::UnaryOperator *AST::LiftPreDecOp(clang::DeclContext *dc, mlir::Operation &op_)
    {
      vast::hl::PreDecOp op = mlir::dyn_cast<vast::hl::PreDecOp>(op_);

      clang::Expr *sub_expr = LiftValue(dc, op.getArg());
      assert(sub_expr != nullptr);
      return CreateUnaryOp(clang::UO_PreDec, sub_expr);
    }

    // TODO(bmt): add more cases
    clang::Stmt *AST::LiftOpImpl(clang::DeclContext *dc, mlir::Operation &op)
    {
      switch (KindOf(op))
      {
      case HlOpKind::kUnknown:
        clang::Stmt *ret;
        llvm::TypeSwitch<mlir::Operation *>(&op)
            .Case([&](vast::core::ScopeOp scope)
                  { ret = LiftScopeOp(dc, op); })
            .Default([&](mlir::Operation *)
                     { std::cout << "No handler for this unknown op!" << endl; 
                     ret=nullptr; });
        return ret;
      case HlOpKind::kBinShlOp:
        return LiftShlOp(dc, op);
      case HlOpKind::kBinAShrOp:
        return LiftAShrOp(dc, op);
      case HlOpKind::kBinLShrOp:
        return LiftLShrOp(dc, op);
      case HlOpKind::kBinAndOp:
        return LiftBinAndOp(dc, op);
      case HlOpKind::kBinOrOp:
        return LiftBinOrOp(dc, op);
      case HlOpKind::kBinXorOp:
        return LiftBinXorOp(dc, op);
      case HlOpKind::kAddIOp:
        return LiftAddIOp(dc, op);
      case HlOpKind::kSubIOp:
        return LiftSubIOp(dc, op);
      case HlOpKind::kMulIOp:
        return LiftMulIOp(dc, op);
      case HlOpKind::kDivUOp:
        return LiftDivUOp(dc, op);
      case HlOpKind::kDivSOp:
        return LiftDivSOp(dc, op);
      case HlOpKind::kRemUOp:
        return LiftRemUOp(dc, op);
      case HlOpKind::kRemSOp:
        return LiftRemSOp(dc, op);
      case HlOpKind::kNotOp:
        return LiftNotOp(dc, op);
      case HlOpKind::kLNotOp:
        return LiftLNotOp(dc, op);
      case HlOpKind::kDeclRefOp:
        return LiftDeclRefOp(dc, op);
      case HlOpKind::kValueYieldOp:
        return LiftValueYieldOp(dc, op);
      case HlOpKind::kExprOp:
        return LiftExprOp(dc, op);
      case HlOpKind::kConstantOp:
        return LiftConstantOp(dc, op);
      case HlOpKind::kImplicitCastOp:
        return LiftImplicitCastOp(dc, op);
      case HlOpKind::kCStyleCastOp:
        return LiftCStyleCastOp(dc, op);
      case HlOpKind::kCondYieldOp:
        return LiftCondYieldOp(dc, op);
      case HlOpKind::kDoOp:
        return LiftDoOp(dc, op);
      case HlOpKind::kReturnOp:
        return LiftReturnOp(dc, op);
      case HlOpKind::kAssignOp:
        return LiftAssignOp(dc, op);
      case HlOpKind::kInitListExpr:
        return LiftInitListExpr(dc, op);
      case HlOpKind::kVarDeclOp:
        return LiftVarDeclOp(dc, op);
      case HlOpKind::kIfOp:
        return LiftIfOp(dc, op);
      case HlOpKind::kWhileOp:
        return LiftWhileOp(dc, op);
      case HlOpKind::kForOp:
        return LiftForOp(dc, op);
      case HlOpKind::kPostIncOp:
        return LiftPostIncOp(dc, op);
      case HlOpKind::kPostDecOp:
        return LiftPostDecOp(dc, op);
      case HlOpKind::kPreIncOp:
        return LiftPreIncOp(dc, op);
      case HlOpKind::kPreDecOp:
        return LiftPreDecOp(dc, op);
      case HlOpKind::kCmpOp:
        return LiftCmpOp(dc, op);
      default:
        std::cout << "No Lifter found for op!\n";
        op.dump();
        return nullptr;
      }
    }

  } // namespace ast
} // namespace pillar
