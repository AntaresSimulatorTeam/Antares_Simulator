#include "antares/solver/utils/named_problem.h"

namespace Antares
{
namespace Optimization
{
using BasisStatus = operations_research::MPSolver::BasisStus;

PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes,
                                                 std::vector<BasisStatus>& StatutDesVariables,
                                                 std::vector<BasisStatus>& StatutDesContraintes) :
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
