#include "named_problem.h"
#include <algorithm>

namespace Antares
{
namespace Optimization
{

PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes,
                                                 const std::vector<bool>& VariablesEntieres,
                                                 std::vector<int>& StatutDesVariables,
                                                 std::vector<int>& StatutDesContraintes,
                                                 bool UseNamedProblems) :

 NomDesVariables(NomDesVariables),
 NomDesContraintes(NomDesContraintes),
 VariablesEntieres(VariablesEntieres),
 StatutDesVariables(StatutDesVariables),
 StatutDesContraintes(StatutDesContraintes),
 useNamedProblems_(UseNamedProblems)
{
}

bool PROBLEME_SIMPLEXE_NOMME::isMIP() const
{
    return std::any_of(
      VariablesEntieres.cbegin(), VariablesEntieres.cend(), [](bool x) { return x; });
}

bool PROBLEME_SIMPLEXE_NOMME::basisExists() const
{
    return !StatutDesVariables.empty() && !StatutDesContraintes.empty();
}

} // namespace Optimization
} // namespace Antares
