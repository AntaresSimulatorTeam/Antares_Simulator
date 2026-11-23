// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <stdexcept>

#include <antares/solver/modeler/ModelerData.h>

namespace Antares::Solver::Checks
{

/// Check that port fields are only used through sum_connections.
void checkPortFields(const ModelerData& data);

// Exposed for unit tests.
void checkModelPortFields(const ModelerStudy::SystemModel::Model& model);

class PortFieldError final: public std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

} // namespace Antares::Solver::Checks
