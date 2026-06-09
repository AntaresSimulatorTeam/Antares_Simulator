// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/modeler/ModelerData.h>

namespace Antares::Solver::Checks
{

/// Check location validity for all modeler data
void checkLocations(const ModelerData& data);

// shouldn't be used outside, here for unit tests
void checkExpression(const Expressions::Nodes::Node* expression,
                     const Config::Location& location,
                     const ModelerStudy::SystemModel::Model& model,
                     const ModelerStudy::SystemModel::System& system,
                     const std::string& exprStr,
                     const std::string& errorMsgForPortFieldSum = "");

class LocationError final: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

} // namespace Antares::Solver::Checks
