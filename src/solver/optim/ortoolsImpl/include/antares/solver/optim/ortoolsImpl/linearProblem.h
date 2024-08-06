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

#include <antares/solver/optim/api/linearProblem.h>

#include <string>

// forward declaration
namespace operations_research
{
    class MPSolver;
    class MPSolverParameters;
    class MPObjective;
}

namespace Antares::Solver::Optim::OrtoolsImpl
{

class OrtoolsLinearProblem: virtual public Api::LinearProblem
{
public:
    OrtoolsLinearProblem(bool isMip, const std::string& solverName);
    ~OrtoolsLinearProblem() = default;

    Api::MipVariable* addNumVariable(double lb, double ub, const std::string& name) override;
    Api::MipVariable* addIntVariable(double lb, double ub, const std::string& name) override;
    Api::MipVariable* addVariable(double lb,
                                  double ub,
                                  bool integer,
                                  const std::string& name) override;
    Api::MipVariable* getVariable(const std::string& name) override;

    Api::MipConstraint* addConstraint(double lb, double ub, const std::string& name) override;
    Api::MipConstraint* getConstraint(const std::string& name) override;

    void setObjectiveCoefficient(Api::MipVariable* var, double coefficient) override;
    double getObjectiveCoefficient(Api::MipVariable* var) override;

    void setMinimization() override;
    void setMaximization() override;

    bool isMinimization() override;
    bool isMaximization() override;

    Api::MipSolution* solve(bool verboseSolver) override;

private:
    std::unique_ptr<operations_research::MPSolver> mpSolver_;
    operations_research::MPSolverParameters* param_;
    operations_research::MPObjective* objective_;

    std::map<std::string, std::unique_ptr<Api::MipVariable>> variables_;
    std::map<std::string, std::unique_ptr<Api::MipConstraint>> constraints_;

    std::unique_ptr<Api::MipSolution> solution_;
};

} // namespace Antares::Solver::Optim::OrtoolsImpl
