#include "NameProvider.h"

std::string NameProvider::FunctionParameterName(vast::hl::FuncOp &func, unsigned argument) const
{
    return "arg_" + std::to_string(argument);
}