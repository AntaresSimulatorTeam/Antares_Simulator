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

#include <map>
#include <ortools/linear_solver/linear_solver.h>
#include <string>
#include <vector>

#include <antares/solver/optim/api/mipSolution.h>
#include <antares/solver/optim/ortoolsImpl/mipObjective.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

class OrtoolsMipSolution: virtual public Api::MipSolution
{
public:
    OrtoolsMipSolution(operations_research::MPSolver::ResultStatus& responseStatus,
                       const std::map<Api::MipVariable*, double>& solution,
                       OrtoolsMipObjective* objective);

    ~OrtoolsMipSolution() = default;

    Api::MipStatus getStatus() override;
    double getObjectiveValue() override;
    double getOptimalValue(const Api::MipVariable& var) const override;
    std::vector<double> getOptimalValue(const std::vector<Api::MipVariable>& vars) const override;

private:
    OrtoolsMipObjective* objective_;
};

} // namespace Antares::Solver::Optim::OrtoolsImpl
