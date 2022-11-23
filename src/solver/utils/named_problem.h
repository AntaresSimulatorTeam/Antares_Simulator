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

public:
    PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                            const std::vector<std::string>& NomDesContraintes,
                            std::vector<int>& StatutDesVariables,
                            std::vector<int>& StatutDesContraintes);

    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    std::vector<int>& StatutDesVariables;
    std::vector<int>& StatutDesContraintes;

    bool isMIP() const;
    bool basisExists() const;
};
} // namespace Optimization
} // namespace Antares
