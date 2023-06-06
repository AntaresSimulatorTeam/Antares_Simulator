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
private:
    std::vector<char*> Variables;
    std::vector<char*> Contraintes;

public:
    PROBLEME_SIMPLEXE_NOMME(std::vector<std::string>& VectNomDesVariables,
                            std::vector<std::string>& VectNomDesContraintes,
                            std::vector<int>& StatutDesVariables,
                            std::vector<int>& StatutDesContraintes);

    char** NomDesVariables;
    char** NomDesContraintes;
    std::vector<int>& StatutDesVariables;
    std::vector<int>& StatutDesContraintes;

    bool isMIP() const;
    bool basisExists() const;
};
} // namespace Optimization
} // namespace Antares
