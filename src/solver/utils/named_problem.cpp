#include "named_problem.h"
#include <algorithm>
#include <iterator>

namespace Antares
{
namespace Optimization
{

char** VectorOfStringToCharPP(std::vector<std::string>& in, std::vector<char*>& pointerVec)
{
    std::transform(in.begin(),
                   in.end(),
                   std::back_inserter(pointerVec),
                   [](std::string& str) { return str.empty() ? nullptr : str.data(); });
    return pointerVec.data();
}
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(std::vector<std::string>& VectNomDesVariables,
                                                 std::vector<std::string>& VectNomDesContraintes,
                                                 std::vector<int>& StatutDesVariables,
                                                 std::vector<int>& StatutDesContraintes) :

 NomDesVariables(VectorOfStringToCharPP(VectNomDesVariables, Variables)),
 NomDesContraintes(VectorOfStringToCharPP(VectNomDesContraintes, Contraintes)),
 StatutDesVariables(StatutDesVariables),
 StatutDesContraintes(StatutDesContraintes)
{
}

bool PROBLEME_SIMPLEXE_NOMME::isMIP() const
{
    // TODO replace implementation when MIP is introduced
    // For now, no problem is MIP.
    return false;
}

bool PROBLEME_SIMPLEXE_NOMME::basisExists() const
{
    return !StatutDesVariables.empty() && !StatutDesContraintes.empty();
}

} // namespace Optimization
} // namespace Antares
