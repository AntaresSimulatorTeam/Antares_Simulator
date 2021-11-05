#include "named_problem.h"

namespace Antares
{
namespace Optimization
{
PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(
  const std::vector<std::string>& NomDesVariables,
  const std::vector<std::string>& NomDesContraintes) :
 NomDesVariables(NomDesVariables), NomDesContraintes(NomDesContraintes)
{
}
} // namespace Optimization
} // namespace Antares
