// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include "VAST.h"

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/SourceMgr.h>
#include <mlir/InitAllDialects.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/OperationSupport.h>
#include <mlir/Parser/Parser.h>
#include <vast/Dialect/Dialects.hpp>
#include <vast/Dialect/HighLevel/HighLevelOps.hpp>
#include <vast/Dialect/HighLevel/HighLevelTypes.hpp>
#include <vast/CodeGen/CodeGenBuilder.hpp>
#include <vast/Dialect/Meta/MetaTypes.hpp>
#include <vast/Dialect/Dialects.hpp>

namespace pillar
{
  namespace
  {

    static llvm::DenseMap<llvm::StringRef, HlOpKind> gNameToKindMap;
    static llvm::DenseMap<mlir::TypeID, HlOpKind> gTypeIDToKindMap;

    class RegistryInitializer
    {
    public:
      mlir::DialectRegistry registry;
      std::vector<mlir::TypeID> op_types;

      RegistryInitializer(void)
      {
        vast::registerAllDialects(registry);
        mlir::registerAllDialects(registry);

#define ADD_OP_TO_MAPS(o)                                        \
  gNameToKindMap.try_emplace(vast::hl::o::getOperationName(),    \
                             HlOpKind::k##o);                    \
  gTypeIDToKindMap.try_emplace(mlir::TypeID::get<vast::hl::o>(), \
                               HlOpKind::k##o);

        HL_DIALECT_OPS(ADD_OP_TO_MAPS)
#undef ADD_OP_TO_MAPS
      }
    };

    static RegistryInitializer gMLIR;

  } // namespace

  HlOpKind KindOf(mlir::Operation *op)
  {
    if (op)
    {
      return KindOf(*op);
    }
    else
    {
      return HlOpKind::kUnknown;
    }
  }

  HlOpKind KindOf(mlir::Operation &op)
  {
    mlir::OperationName op_name = op.getName();
    if (auto info = op_name.getRegisteredInfo())
    {
      mlir::TypeID type_id = info->getTypeID();
      if (auto it = gTypeIDToKindMap.find(type_id);
          it != gTypeIDToKindMap.end())
      {
        return it->second;
      }
      else
      {
        return HlOpKind::kUnknown;
      }
    }

    if (!mlir::isa<vast::hl::HighLevelDialect>(op.getDialect()))
    {
      return HlOpKind::kUnknown;
    }

    if (auto it = gNameToKindMap.find(op_name.getStringRef());
        it != gNameToKindMap.end())
    {
      return it->second;
    }

    return HlOpKind::kUnknown;
  }

  VASTModuleImpl::VASTModuleImpl(void)
      : context(gMLIR.registry) {}

  VASTModuleImpl::~VASTModuleImpl(void) {}

  VASTModule::~VASTModule(void) {}

  std::optional<VASTModule> VASTModule::Deserialize(std::string_view data)
  {
    std::shared_ptr<VASTModuleImpl> impl = std::make_shared<VASTModuleImpl>();
    llvm::SourceMgr sm;
    auto buffer = llvm::MemoryBuffer::getMemBuffer(data);
    sm.AddNewSourceBuffer(std::move(buffer), llvm::SMLoc());
    impl->module = mlir::parseSourceFile<mlir::ModuleOp>(sm, &(impl->context));
    if (!impl->module)
    {
      return std::nullopt;
    }

    return VASTModule(std::move(impl));
  }

} // namespace pillar
