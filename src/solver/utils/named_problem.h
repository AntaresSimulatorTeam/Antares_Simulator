#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include <vector>
#include <string>

namespace Antares
{
namespace Optimization
{
struct PROBLEME_SIMPLEXE_NOMME : public PROBLEME_SIMPLEXE
{
    PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                            const std::vector<std::string>& NomDesContraintes,
                            const std::vector<bool>& VariablesEntieres);

    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    const std::vector<bool>& VariablesEntieres;
    std::vector<MPSolver::BasisStatus> StatutDesVariables;
    std::vector<MPSolver::BasisStatus> StatutDesContraintes;
    
    bool isMIP() const;
};
} // namespace Optimization
} // namespace Antares
