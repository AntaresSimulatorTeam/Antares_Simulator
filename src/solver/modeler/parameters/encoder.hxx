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

#include <antares/solver/modeler/parameters/modelerParameters.h>

#include "yaml-cpp/yaml.h"

namespace YAML
{
template<>
struct convert<Antares::Solver::ModelerParameters>
{
    static bool decode(const Node& node, Antares::Solver::ModelerParameters& rhs)
    {
        if (!node.IsMap())
        {
            return false;
        }
        rhs.solver = node["solver"].as<std::string>();
        rhs.solverLogs = node["solver-logs"].as<bool>(false);
        rhs.solverParameters = node["solver-parameters"].as<std::string>();
        rhs.noOutput = node["no-output"].as<bool>(false);
        rhs.firstTimeStep = node["first-time-step"].as<unsigned int>(0);
        // TODO check this value
        rhs.lastTimeStep = node["last-time-step"].as<unsigned int>(167);
        return true;
    }
};
} // namespace YAML
