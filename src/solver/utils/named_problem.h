#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

namespace Antares
{
namespace Optimization
{
static inline char** VectorOfStringToCharPP(std::vector<std::string>& in,
                                            std::vector<char*>& pointerVec)
{
    std::transform(in.begin(),
                   in.end(),
                   std::back_inserter(pointerVec),
                   [](std::string& str) { return str.empty() ? nullptr : str.data(); });
    return pointerVec.data();
}
struct PROBLEME_SIMPLEXE_NOMME : public PROBLEME_SIMPLEXE
{
public:
    PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                            const std::vector<std::string>& NomDesContraintes,
                            std::vector<int>& StatutDesVariables,
                            std::vector<int>& StatutDesContraintes);

private:
    std::vector<std::string> NomDesVariables;
    std::vector<std::string> NomDesContraintes;

public:
    std::vector<int>& StatutDesVariables;
    std::vector<int>& StatutDesContraintes;

    bool isMIP() const;
    bool basisExists() const;
    char** VariableNamesAsCharPP(std::vector<char*>& pointerVec)
    {
        return VectorOfStringToCharPP(NomDesVariables, pointerVec);
    }
    char** ConstraintNamesAsCharPP(std::vector<char*>& pointerVec)
    {
        return VectorOfStringToCharPP(NomDesContraintes, pointerVec);
    }
    const std::vector<std::string>& VariableNames() const
    {
        return NomDesVariables;
    }
    const std::vector<std::string>& ConstraintNames() const
    {
        return NomDesContraintes;
    }
};
} // namespace Optimization
} // namespace Antares
