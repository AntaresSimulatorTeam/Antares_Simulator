#pragma once

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "ortools/linear_solver/linear_solver.h"

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
                            const std::vector<bool>& VariablesEntieres,
                            std::vector<BasisStatus>& StatutDesVariables,
                            std::vector<BasisStatus>& StatutDesContraintes,
                            bool UseNamedProblems,
                            bool SolverLogs);

private:
    const std::vector<std::string>& NomDesVariables;
    const std::vector<std::string>& NomDesContraintes;
    bool useNamedProblems_;
    bool solverLogs_;

public:
    std::vector<BasisStatus>& StatutDesVariables;
    std::vector<BasisStatus>& StatutDesContraintes;
    const std::vector<bool>& VariablesEntieres;

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

    bool SolverLogs() const
    {
        return solverLogs_;
    }

    bool IntegerVariable(size_t idx) const
    {
        return VariablesEntieres[idx];
    }
};
} // namespace Optimization
} // namespace Antares
