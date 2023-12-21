#include "Clang.h"
#include "VAST.h"
#include <vast/Dialect/HighLevel/HighLevelOps.hpp>
#include <vast/Dialect/HighLevel/HighLevelTypes.hpp>
#include <vast/Interfaces/TypeQualifiersInterfaces.hpp>

class NameProvider
{
public:
    virtual ~NameProvider(void) = default;

    // virtual std::string StructureName(mlir::Operation *tag) const;
    // virtual std::string VariableName(mlir::Operation *var) const;
    // virtual std::string FunctionName(mlir::Operation *func) const;
    virtual std::string FunctionParameterName(vast::hl::FuncOp &func, unsigned argument) const;
};