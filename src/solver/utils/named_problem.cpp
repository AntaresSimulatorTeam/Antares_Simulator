#include "named_problem.h"

namespace Antares
{
namespace Optimization
{
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(char** NomDesVariables,
                                                 char** NomDesContraintes,
                                                 std::vector<int>& StatutDesVariables,
                                                 std::vector<int>& StatutDesContraintes) :
 NomDesVariables(NomDesVariables),
 NomDesContraintes(NomDesContraintes),
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
