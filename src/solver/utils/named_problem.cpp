#include "named_problem.h"
#include <algorithm>

namespace Antares
{
namespace Optimization
{
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes,
                                                 const std::vector<bool>& VariablesEntieres,
                                                 const double* ValeursDesVariablesFixees,
                                                 const int* ColonnesFixees,
                                                 const int NombreDeVariablesFixees) :
 NomDesVariables(NomDesVariables),
 NomDesContraintes(NomDesContraintes),
 VariablesEntieres(VariablesEntieres),
 solveOnlyRelaxation(true),
 ValeursDesVariablesFixees(ValeursDesVariablesFixees),
 ColonnesFixees(ColonnesFixees),
 NombreDeVariablesFixees(NombreDeVariablesFixees)
{
}

bool PROBLEME_SIMPLEXE_NOMME::isMIP() const
{
    return std::any_of(VariablesEntieres.cbegin(), VariablesEntieres.cend(), [](bool x) { return x; });
}

} // namespace Optimization
} // namespace Antares
