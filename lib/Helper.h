#include "AST.h"

std::pair<clang::Type *, uint32_t> getQualSet(clang::QualType qualType)
{
    return {qualtype.getTypePtr(), qualtype.getQualifiers().getAsOpaqueValue()};
}
