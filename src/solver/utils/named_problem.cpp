#include "named_problem.h"
#include <algorithm>

namespace Antares::Optimization
{

PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes,
                                                 const std::vector<bool>& VariablesEntieres,
                                                 std::vector<int>& StatutDesVariables,
                                                 std::vector<int>& StatutDesContraintes,
                                                 bool UseNamedProblems,
                                                 bool SolverLogs) : PROBLEME_SIMPLEXE(),

 NomDesVariables(NomDesVariables),
 NomDesContraintes(NomDesContraintes),
 useNamedProblems_(UseNamedProblems),
 solverLogs_(SolverLogs),
 StatutDesVariables(StatutDesVariables),
 StatutDesContraintes(StatutDesContraintes),
 VariablesEntieres(VariablesEntieres)
{
    AffichageDesTraces = SolverLogs ? OUI_SPX : NON_SPX;
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

} // namespace Antares::Optimization
