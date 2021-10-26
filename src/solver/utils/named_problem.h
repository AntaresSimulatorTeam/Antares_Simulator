#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"


struct PROBLEME_SIMPLEXE_NOMME : public PROBLEME_SIMPLEXE {
  PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                 const std::vector<std::string>& NomDesContraintes) : NomDesVariables(NomDesVariables), NomDesContraintes(NomDesContraintes) {}

  const std::vector<std::string> NomDesVariables;
  const std::vector<std::string> NomDesContraintes;
};
