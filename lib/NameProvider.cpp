#include "NameProvider.h"
#include "Clang.h"
#include "VAST.h"
#include <vast/Dialect/HighLevel/HighLevelOps.hpp>
#include <vast/Dialect/HighLevel/HighLevelTypes.hpp>
#include <vast/Interfaces/TypeQualifiersInterfaces.hpp>

std::string NameProvider::FunctionParameterName(vast::hl::FuncOp &func, unsigned argument) const
{
    return "arg_" + std::to_string(argument);
}

std::string NameProvider::FunctionName(vast::hl::FuncOp &func) const
{
    return func.getName().str();
}

std::string NameProvider::VariableName(vast::hl::VarDeclOp &var) const
{
    return var.getName().str();
}

std::string NameProvider::StructName(vast::hl::StructDeclOp &strct) const
{
    return strct.getName().str();
}

std::string NameProvider::EnumName(vast::hl::EnumDeclOp &en) const
{
    return en.getName().str();
}
std::string NameProvider::EnumConstantName(vast::hl::EnumConstantOp &en) const
{
    return en.getName().str();
}

std::string NameProvider::TypeDefName(vast::hl::TypeDefOp &type_def_op) const
{
    return type_def_op.getName().str();
}
