// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "AST.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbitfield-enum-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Attr.h>
#include <clang/AST/Decl.h>
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
#include <string>
#include <vast/Dialect/HighLevel/HighLevelOps.hpp>
#include <vast/Dialect/HighLevel/HighLevelTypes.hpp>
#include <vast/Interfaces/TypeQualifiersInterfaces.hpp>
#include <vast/Util/TypeSwitch.hpp>

namespace pillar
{
  namespace ast
  {

    bool AST::CharIsUnsigned(void) const
    {
      auto bty = clang::dyn_cast<clang::BuiltinType>(ctx.CharTy.getTypePtr());
      switch (bty->getKind())
      {
      case clang::BuiltinType::Char_U:
      case clang::BuiltinType::UChar:
        return true;
      case clang::BuiltinType::Char_S:
      case clang::BuiltinType::SChar:
        return false;
      default:
        assert(false);
        return false;
      }
    }

    // Port VAST hl qualifiers into a Clang qualified type.
    clang::QualType AST::Qualify(clang::QualType ty,
                                 vast::hl::UCVQualifiersAttr quals)
    {
      if (!quals)
      {
        return ty;
      }
      if (quals.hasConst())
      {
        ty = ty.withConst();
      }
      if (quals.hasVolatile())
      {
        ty = ty.withVolatile();
      }
      return ty;
    }

    // Port VAST hl qualifiers into a Clang qualified type.
    clang::QualType AST::Qualify(clang::QualType ty,
                                 vast::hl::CVRQualifiersAttr quals)
    {
      if (!quals)
      {
        return ty;
      }
      if (quals.hasConst())
      {
        ty = ty.withConst();
      }
      if (quals.hasVolatile())
      {
        ty = ty.withVolatile();
      }
      if (quals.hasRestrict())
      {
        ty = ty.withRestrict();
      }
      return ty;
    }

    // Port VAST hl qualifiers into a Clang qualified type.
    clang::QualType AST::Qualify(clang::QualType ty,
                                 vast::hl::CVQualifiersAttr quals)
    {
      if (!quals)
      {
        return ty;
      }
      if (quals.hasConst())
      {
        ty = ty.withConst();
      }
      if (quals.hasVolatile())
      {
        ty = ty.withVolatile();
      }
      return ty;
    }

    clang::QualType AST::LiftType(mlir::Type ty)
    {
      if (auto it = type_map.find(ty); it != type_map.end())
      {
        return it->second;
      }

#define HL_TYPE_CASE(hl_type_name, var_name, ...) \
  Case<vast::hl::hl_type_name>(                   \
      [ =, this ](vast::hl::hl_type_name var_name) -> clang::QualType __VA_ARGS__)

#define HL_BUILTIN_TYPE_CASE(hl_type_name, ctx_type_name) \
  HL_TYPE_CASE(hl_type_name, bty, {                       \
    return Qualify(ctx.ctx_type_name, bty.getQuals());    \
  })

#define HL_BUILTIN_INT_CASE(hl_type_name, signed_type_name, unsigned_type_name) \
  HL_TYPE_CASE(hl_type_name, ity, {                                             \
    vast::hl::UCVQualifiersAttr ucv = ity.getQuals();                           \
    if (ucv && ucv.hasUnsigned())                                               \
    {                                                                           \
      return Qualify(ctx.unsigned_type_name, ucv);                              \
    }                                                                           \
    else                                                                        \
    {                                                                           \
      return Qualify(ctx.signed_type_name, ucv);                                \
    }                                                                           \
  })

      return type_map.try_emplace(
                         ty, vast::TypeSwitch<mlir::Type, clang::QualType>(ty)
                                 .HL_BUILTIN_TYPE_CASE(VoidType, VoidTy)
                                 .HL_BUILTIN_TYPE_CASE(BoolType, BoolTy)
                                 .HL_TYPE_CASE(CharType, cty, {
            vast::hl::UCVQualifiersAttr ucv = cty.getQuals();
            if (ucv && ucv.hasUnsigned()) {
              if (char_is_unsigned) {
                return Qualify(ctx.CharTy, ucv);
              } else {
                return Qualify(ctx.UnsignedCharTy, ucv);
              }
            } else if (char_is_unsigned) {
              return Qualify(ctx.SignedCharTy, ucv);
            } else {
              return Qualify(ctx.CharTy, ucv);
            } })
                                 .HL_BUILTIN_INT_CASE(ShortType, ShortTy, UnsignedShortTy)
                                 .HL_BUILTIN_INT_CASE(IntType, IntTy, UnsignedIntTy)
                                 .HL_BUILTIN_INT_CASE(LongType, LongTy, UnsignedLongTy)
                                 .HL_BUILTIN_INT_CASE(LongLongType, LongLongTy, UnsignedLongLongTy)
                                 .HL_BUILTIN_INT_CASE(Int128Type, Int128Ty, UnsignedInt128Ty)
                                 .HL_BUILTIN_TYPE_CASE(HalfType, HalfTy)
                                 .HL_BUILTIN_TYPE_CASE(BFloat16Type, BFloat16Ty)
                                 .HL_BUILTIN_TYPE_CASE(FloatType, FloatTy)
                                 .HL_BUILTIN_TYPE_CASE(DoubleType, DoubleTy)
                                 .HL_BUILTIN_TYPE_CASE(LongDoubleType, LongDoubleTy)
                                 .HL_BUILTIN_TYPE_CASE(Float128Type, Float128Ty)
                                 .HL_TYPE_CASE(PointerType, pty, { return Qualify(ctx.getPointerType(LiftType(pty.getElementType())),
                                                                                  pty.getQuals()); })
                                 .HL_TYPE_CASE(ArrayType, aty, {
            clang::QualType rty;
            clang::QualType ety = LiftType(aty.getElementType());
            if (auto size = aty.getSize()) {
              llvm::APInt size_i(64, size.value());
              rty = ctx.getConstantArrayType(
                  ety, size_i, nullptr,
                  clang::ArrayType::ArraySizeModifier::Normal, 0u);
            } else {
              rty = ctx.getIncompleteArrayType(
                  ety, clang::ArrayType::ArraySizeModifier::Normal, 0u);
            }
            return Qualify(rty, aty.getQuals()); })
                                 .HL_TYPE_CASE(LValueType, lty, { return LiftType(lty.getElementType()); })
                                 .Case([=, this](mlir::FunctionType fty) -> clang::QualType
                                       { return LiftFunctionType(fty); })
                                 .Default([=, this](auto t) -> clang::QualType
                                          {
                                            (void) this;
          t.dump();
          assert(false);
          return {}; }))
          .first->second;

#undef HL_TYPE_CASE
#undef HL_BUILTIN_TYPE_CASE
#undef HL_BUILTIN_INT_CASE
    }

    clang::QualType AST::LiftFunctionType(mlir::FunctionType ty)
    {
      auto num_results = ty.getNumResults();
      clang::QualType ret_type = ctx.VoidTy;
      if (num_results)
      {
        ret_type = LiftType(ty.getResult(0u));
        assert(1u == num_results);
      }
      llvm::SmallVector<clang::QualType, 8> arg_types;
      for (auto i = 0u, max_i = ty.getNumInputs(); i < max_i; ++i)
      {
        arg_types.emplace_back(LiftType(ty.getInput(i)));
      }

      clang::FunctionProtoType::ExtProtoInfo epi;
      // TODO(pag): variadic info.
      return ctx.getFunctionType(ret_type, arg_types, epi);
    }

  } // namespace ast
} // namespace pillar
