// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "Clang.h"

#include <cassert>
#include <string>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbitfield-enum-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#include <clang/Tooling/Tooling.h>
// #include <clang/AST/ASTConsumer.h>
// #include <clang/Basic/Builtins.h>
// #include <clang/Basic/Diagnostic.h>
// #include <clang/Basic/DiagnosticIDs.h>
// #include <clang/Basic/DiagnosticOptions.h>
// #include <clang/Basic/LangOptions.h>
// #include <clang/Basic/SourceLocation.h>
// #include <clang/Basic/SourceManager.h>
// #include <clang/Basic/TargetInfo.h>
// #include <clang/Basic/TargetOptions.h>
// #include <clang/Lex/Preprocessor.h>
// #include <clang/Lex/PreprocessorOptions.h>
// #include <clang/Sema/Lookup.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Support/CrashRecoveryContext.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/TargetSelect.h>
#pragma GCC diagnostic pop

#include "AST.h"

namespace pillar
{
  namespace
  {

    static constexpr clang::SourceLocation kEmptyLoc;

    class LLVMInitializer
    {
    public:
      LLVMInitializer(void)
      {
        // We're a library, and we *don't* want LLVM to do any crash recovery on
        // our behalf.
        llvm::CrashRecoveryContext::Disable();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmPrinters();
        llvm::InitializeAllAsmParsers();
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
      }

      ~LLVMInitializer(void)
      {
        llvm::llvm_shutdown();
      }
    };

    static LLVMInitializer gLLVM;

    static std::vector<std::string> Arguments(const llvm::Triple &triple)
    {
      std::vector<std::string> args;
      args.emplace_back("-target");
      args.push_back(triple.normalize());
      return args;
    }

    enum CExprPrecedence : unsigned
    {
      Value = 0U,
      SpecialOp,
      UnaryOp,
      BinaryOp = UnaryOp + clang::UO_LNot + 1U,
      CondOp = BinaryOp + clang::BO_Comma + 1U
    };

    static unsigned GetOperatorPrecedence(clang::UnaryOperatorKind opc)
    {
      return static_cast<unsigned>(CExprPrecedence::UnaryOp) +
             static_cast<unsigned>(opc);
    }

    static unsigned GetOperatorPrecedence(clang::BinaryOperatorKind opc)
    {
      return static_cast<unsigned>(CExprPrecedence::BinaryOp) +
             static_cast<unsigned>(opc);
    }

    static unsigned GetOperatorPrecedence(clang::Expr *op)
    {
      if (auto cast = clang::dyn_cast<clang::ImplicitCastExpr>(op))
      {
        return GetOperatorPrecedence(cast->getSubExpr());
      }

      if (clang::isa<clang::DeclRefExpr>(op) ||
          clang::isa<clang::IntegerLiteral>(op) ||
          clang::isa<clang::FloatingLiteral>(op) ||
          clang::isa<clang::InitListExpr>(op) ||
          clang::isa<clang::CompoundLiteralExpr>(op) ||
          clang::isa<clang::ParenExpr>(op) ||
          clang::isa<clang::StringLiteral>(op))
      {
        return CExprPrecedence::Value;
      }

      if (clang::isa<clang::MemberExpr>(op) ||
          clang::isa<clang::ArraySubscriptExpr>(op) ||
          clang::isa<clang::CallExpr>(op))
      {
        return CExprPrecedence::SpecialOp;
      }

      if (clang::isa<clang::CStyleCastExpr>(op))
      {
        return CExprPrecedence::UnaryOp;
      }

      if (auto uo = clang::dyn_cast<clang::UnaryOperator>(op))
      {
        return GetOperatorPrecedence(uo->getOpcode());
      }

      if (auto bo = clang::dyn_cast<clang::BinaryOperator>(op))
      {
        return GetOperatorPrecedence(bo->getOpcode());
      }

      if (clang::isa<clang::ConditionalOperator>(op))
      {
        return CExprPrecedence::CondOp;
      }

      assert(false);
      return 0U;
    }

  } // namespace

  ClangModule::~ClangModule(void) {}

  std::optional<ClangModule> ClangModule::Lift(const VASTModule &module)
  {
    std::shared_ptr<mlir::Operation> op(module.impl,
                                        module.impl->module->getOperation());
    if (auto ptr = ast::AST::CreateFromModule(std::move(op)))
    {
      return ClangModule(ptr);
    }
    else
    {
      return std::nullopt;
    }
  }

  ClangModuleImpl::~ClangModuleImpl(void)
  {
    (void)llvm;
  }

  ClangModuleImpl::ClangModuleImpl(const llvm::Triple &triple)
      : llvm(&gLLVM),
        unit(clang::tooling::buildASTFromCodeWithArgs("", Arguments(triple),
                                                      "pillar.c")),
        ctx(unit->getASTContext()),
        sema(unit->getSema()) {}

  clang::IdentifierInfo *ClangModuleImpl::CreateIdentifier(
      const llvm::StringRef &str)
  {
    return &ctx.Idents.get(str);
  }

  clang::FunctionDecl *ClangModuleImpl::CreateFunctionDecl(
      clang::DeclContext *sdc, clang::DeclContext *ldc,
      const clang::QualType &type, clang::IdentifierInfo *id)
  {
    auto fd = clang::FunctionDecl::Create(
        ctx, sdc, kEmptyLoc, kEmptyLoc,
        clang::DeclarationName(id), type, {},
        clang::SC_None, /*isInlineSpecified=*/false);
    if (sdc != ldc)
    {
      fd->setLexicalDeclContext(ldc);
    }
    return fd;
  }
  clang::VarDecl *ClangModuleImpl::CreateVarDecl(clang::DeclContext *sdc, clang::DeclContext *ldc,
                                                 const clang::QualType &type, clang::IdentifierInfo *id)
  {
    clang::VarDecl *clangVarDecl =
        clang::VarDecl::Create(ctx, sdc, kEmptyLoc, kEmptyLoc,
                               id, type, nullptr,
                               clang::SC_None);
    if (sdc != ldc)
    {
      clangVarDecl->setLexicalDeclContext(ldc);
    }

    return clangVarDecl;
  }

  clang::DoStmt *ClangModuleImpl::CreateDo(clang::Expr *cond, clang::Stmt *body)
  {
    clang::ExprResult er = sema.CheckBooleanCondition(kEmptyLoc, cond);
    assert(er.isUsable());
    er = sema.ActOnFinishFullExpr(er.get(), kEmptyLoc, /*DiscardedValue=*/false);
    assert(er.isUsable());
    return new (ctx) clang::DoStmt(body, er.get(), kEmptyLoc, kEmptyLoc,
                                   kEmptyLoc);
  }

  clang::DeclRefExpr *ClangModuleImpl::CreateDeclRef(clang::ValueDecl *val)
  {
    clang::DeclarationNameInfo dni(val->getDeclName(), kEmptyLoc);
    clang::CXXScopeSpec ss;
    clang::ExprResult er = sema.BuildDeclarationNameExpr(ss, dni, val);
    assert(er.isUsable());
    clang::DeclRefExpr *dre = er.getAs<clang::DeclRefExpr>();
    assert(dre != nullptr);
    return dre;
  }

  clang::ParenExpr *ClangModuleImpl::CreateParen(clang::Expr *expr)
  {
    return new (ctx) clang::ParenExpr(kEmptyLoc, kEmptyLoc, expr);
  }

  clang::UnaryOperator *ClangModuleImpl::CreateUnaryOp(
      clang::UnaryOperatorKind opc, clang::Expr *expr)
  {

    if (GetOperatorPrecedence(opc) < GetOperatorPrecedence(expr))
    {
      expr = CreateParen(expr);
    }

    clang::ExprResult er = sema.CreateBuiltinUnaryOp(kEmptyLoc, opc, expr);
    assert(er.isUsable());
    clang::UnaryOperator *uo = er.getAs<clang::UnaryOperator>();
    assert(uo != nullptr);
    return uo;
  }

  clang::BinaryOperator *ClangModuleImpl::CreateBinaryOp(
      clang::BinaryOperatorKind opc, clang::Expr *lhs,
      clang::Expr *rhs)
  {
    if (GetOperatorPrecedence(opc) < GetOperatorPrecedence(lhs))
    {
      lhs = CreateParen(lhs);
    }
    if (GetOperatorPrecedence(opc) < GetOperatorPrecedence(rhs))
    {
      rhs = CreateParen(rhs);
    }
    clang::ExprResult er = sema.CreateBuiltinBinOp(kEmptyLoc, opc, lhs, rhs);
    assert(er.isUsable());
    clang::BinaryOperator *bo = er.getAs<clang::BinaryOperator>();
    assert(bo != nullptr);
    return bo;
  }

  clang::ReturnStmt *ClangModuleImpl::CreateReturn(clang::Expr *val)
  {
    return clang::ReturnStmt::Create(ctx, kEmptyLoc, val, nullptr);
  }

} // namespace pillar
