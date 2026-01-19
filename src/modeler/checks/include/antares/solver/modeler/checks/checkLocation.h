// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
