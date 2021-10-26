#include "named_problem.h"

#include <cassert>

PROBLEME_SIMPLEXE_NOMME::PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                                                 const std::vector<std::string>& NomDesContraintes) : NomDesVariables(NomDesVariables), NomDesContraintes(NomDesContraintes) {
  assert(NomDesVariables.size() == NombreDeVariables);
  assert(NomDesContraintes.size() == NombreDeContraintes);
}
