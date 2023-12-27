#include <string>
namespace vast::hl
{
    class FuncOp;
    class VarDeclOp;
    class CxxStructDeclOp;
    class EnumDeclOp;
    class EnumConstantOp;
    class TypeDefOp;
}

class NameProvider
{
public:
    virtual ~NameProvider(void) = default;

    virtual std::string FunctionParameterName(vast::hl::FuncOp &func, unsigned argument) const;
    virtual std::string FunctionName(vast::hl::FuncOp &func) const;
    virtual std::string VariableName(vast::hl::VarDeclOp &var) const;
    virtual std::string StructName(vast::hl::CxxStructDeclOp &strct) const;
    virtual std::string EnumName(vast::hl::EnumDeclOp &en) const;
    virtual std::string EnumConstantName(vast::hl::EnumConstantOp &en) const;
    virtual std::string TypeDefName(vast::hl::TypeDefOp &type_def_op) const;
};