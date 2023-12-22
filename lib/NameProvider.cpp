#include "NameProvider.h"

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

std::string NameProvider::StructName(vast::hl::CxxStructDeclOp &strct) const
{
    return strct.getName().str();
}

std::string NameProvider::EnumName(vast::hl::EnumDeclOp &en) const
{
    return en.getName().str();
}

std::string NameProvider::TypeDefName(vast::hl::TypeDefOp &tyDef) const
{
    return tyDef.getName().str();
}
