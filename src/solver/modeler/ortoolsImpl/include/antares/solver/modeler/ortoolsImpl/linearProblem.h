/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <antares/solver/modeler/api/linearProblem.h>
#include <antares/solver/modeler/ortoolsImpl/mipConstraint.h>
#include <antares/solver/modeler/ortoolsImpl/mipSolution.h>
#include <antares/solver/modeler/ortoolsImpl/mipVariable.h>

namespace operations_research
{
class MPSolver;
class MPSolverParameters;
class MPObjective;
} // namespace operations_research

namespace Antares::Solver::Modeler::OrtoolsImpl
{

class OrtoolsLinearProblem: public Api::ILinearProblem
{
public:
    OrtoolsLinearProblem(bool isMip, const std::string& solverName);
    ~OrtoolsLinearProblem() override = default;

    OrtoolsMipVariable* addNumVariable(double lb, double ub, const std::string& name) override;
    OrtoolsMipVariable* addIntVariable(double lb, double ub, const std::string& name) override;
    OrtoolsMipVariable* addVariable(double lb,
                                    double ub,
                                    bool integer,
                                    const std::string& name) override;
    OrtoolsMipVariable* getVariable(const std::string& name) const override;
    int variableCount() const override;

    OrtoolsMipConstraint* addConstraint(double lb, double ub, const std::string& name) override;
    OrtoolsMipConstraint* getConstraint(const std::string& name) const override;
    int constraintCount() const override;

    void setObjectiveCoefficient(Api::IMipVariable* var, double coefficient) override;
    double getObjectiveCoefficient(const Api::IMipVariable* var) const override;

    void setMinimization() override;
    void setMaximization() override;

    bool isMinimization() const override;
    bool isMaximization() const override;

    OrtoolsMipSolution* solve(bool verboseSolver) override;
    void WriteLP(const std::string& filename) override;

    double infinity() const override;

protected:
    operations_research::MPSolver* MpSolver() const;

private:
    operations_research::MPSolver* mpSolver_;
    operations_research::MPObjective* objective_;
    operations_research::MPSolverParameters params_;

    std::map<std::string, std::unique_ptr<OrtoolsMipVariable>> variables_;
    std::map<std::string, std::unique_ptr<OrtoolsMipConstraint>> constraints_;

    std::unique_ptr<OrtoolsMipSolution> solution_;
};

} // namespace Antares::Solver::Modeler::OrtoolsImpl
