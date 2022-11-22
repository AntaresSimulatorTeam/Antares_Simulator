#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include <ortools/linear_solver/linear_solver.h>
#include <vector>
#include <string>

namespace Antares
{
namespace Optimization
{
struct PROBLEME_SIMPLEXE_NOMME : public PROBLEME_SIMPLEXE
{
private:
    using BasisStatus = operations_research::MPSolver::BasisStatus;

public:
    PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                            const std::vector<std::string>& NomDesContraintes,
                            std::vector<BasisStatus>& StatutDesVariables,
                            std::vector<BasisStatus>& StatutDesContraintes);

    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    std::vector<BasisStatus>& StatutDesVariables;
    std::vector<BasisStatus>& StatutDesContraintes;

    bool isMIP() const;
    bool basisExists() const;
};
} // namespace Optimization
} // namespace Antares
