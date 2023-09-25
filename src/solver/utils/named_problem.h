#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

namespace Antares
{
namespace Optimization
{
struct PROBLEME_SIMPLEXE_NOMME : public PROBLEME_SIMPLEXE
{
public:
    PROBLEME_SIMPLEXE_NOMME(const std::vector<std::string>& NomDesVariables,
                            const std::vector<std::string>& NomDesContraintes,
                            std::vector<int>& StatutDesVariables,
                            std::vector<int>& StatutDesContraintes,
                            bool UseNamedProblems);

private:
    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    bool useNamedProblems_;

public:
    std::vector<int>& StatutDesVariables;
    std::vector<int>& StatutDesContraintes;

    bool isMIP() const;
    bool basisExists() const;

    bool UseNamedProblems() const
    {
        return useNamedProblems_;
    }

    void SetUseNamedProblems(bool useNamedProblems)
    {
        useNamedProblems_ = useNamedProblems;
    }

    const std::vector<std::string>& VariableNames() const
    {
        return NomDesVariables;
    }

    const std::vector<std::string>& ConstraintNames() const
    {
        return NomDesContraintes;
    }
};
} // namespace Optimization
} // namespace Antares
