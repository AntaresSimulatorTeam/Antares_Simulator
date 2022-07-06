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
                            const std::vector<bool>& VariablesEntieres,
                            const double* ValeursDesVariablesFixees,
                            const int* ColonnesFixees,
                            const int NombreDeVariablesFixees);

    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    const std::vector<bool>& VariablesEntieres;

    const double* ValeursDesVariablesFixees;
    const int* ColonnesFixees;
    const int NombreDeVariablesFixees;

    bool solveOnlyRelaxation;

    bool isMIP() const;
};
} // namespace Optimization
} // namespace Antares
