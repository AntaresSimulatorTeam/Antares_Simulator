/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/modeler/data.h>

namespace Antares::Modeler::Checks
{

/// Check location validity for all modeler data
void checkLocations(const Antares::Modeler::Data& data);

// shouldn't be used outside, here for unit tests
void checkExpression(const Antares::Expressions::Nodes::Node* expression,
                     const Antares::Modeler::Config::Location& location,
                     const Antares::ModelerStudy::SystemModel::Model& model,
                     const Antares::ModelerStudy::SystemModel::System& system,
                     const std::string& exprStr,
                     const std::string& errorMsgForPortFieldSum = "");

class LocationError final: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

} // namespace Antares::Modeler::Checks
