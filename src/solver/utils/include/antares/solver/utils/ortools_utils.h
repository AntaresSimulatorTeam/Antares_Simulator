/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <map>
#include <string>

#include <antares/writer/i_writer.h>

// ignore unused parameters warnings from ortools
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

#include "ortools_wrapper.h"

using namespace operations_research;

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver,
                                                   Antares::Solver::IResultWriter& writer,
                                                   const std::string& filename);

/*!
 *  \brief Return list of available ortools solver name on our side
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();

/*!
 *  \brief Return a single string containing all solvers available, separated by a ", " and ending
 * with a ".".
 *
 */
std::string availableOrToolsSolversString();

/*!
 *  \brief Create a MPSolver with correct linear or mixed variant
 *
 *  \return MPSolver
 */
MPSolver* MPSolverFactory(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* probleme,
                          const std::string& solverName);

std::string generateTempPath(const std::string& filename);
void removeTemporaryFile(const std::string& tmpPath);

class OrtoolsUtils
{
public:
    struct SolverNames
    {
        std::string LPSolverName, MIPSolverName;
    };
    static const std::map<std::string, struct SolverNames> solverMap;
};

namespace Antares
{
namespace Optimization
{

class Nomenclature
{
public:
    Nomenclature() = delete;

    explicit Nomenclature(char prefix):
        prefix_(prefix)
    {
    }

    void SetTarget(const std::vector<std::string>& target)
    {
        target_ = &target;
    }

    std::string GetName(unsigned index) const
    {
        if (target_ == nullptr || target_->at(index).empty())
        {
            return prefix_ + std::to_string(index);
        }
        return target_->at(index);
    }

private:
    const std::vector<std::string>* target_ = nullptr;
    char prefix_;
};

class ProblemSimplexeNommeConverter
{
public:
    explicit ProblemSimplexeNommeConverter(
      const std::string& solverName,
      const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);

    MPSolver* Convert();

private:
    const std::string& solverName_;
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;
    Nomenclature variableNameManager_ = Nomenclature('x');
    Nomenclature constraintNameManager_ = Nomenclature('c');

    void CreateVariable(unsigned idxVar, MPSolver* solver, MPObjective* const objective) const;
    void CopyVariables(MPSolver* solver) const;
    void UpdateContraints(unsigned idxRow, MPSolver* solver) const;
    void CopyRows(MPSolver* solver) const;
    void CopyMatrix(const MPSolver* solver) const;
};
} // namespace Optimization
} // namespace Antares
