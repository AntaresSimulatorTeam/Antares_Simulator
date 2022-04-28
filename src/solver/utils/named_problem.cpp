#include "named_problem.h"

namespace Antares
{
namespace Optimization
{
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(
  const std::vector<std::string>& NomDesVariables,
  const std::vector<std::string>& NomDesContraintes,
  const std::vector<bool>& VariablesEntieres) :
 NomDesVariables(NomDesVariables), 
 NomDesContraintes(NomDesContraintes),
 VariablesEntieres(VariablesEntieres)
{
}
} // namespace Optimization
} // namespace Antares
