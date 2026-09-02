// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Solver::Variable
{
// Registers the concrete list of thematic-trimming output variables with
// Antares::Data::ThematicTrimmingVariableRegistry. Must be called once, before any
// Antares::Data::Parameters::reset()/loadFromINI() call, by every executable that loads a
// study (see src/solver/main.cpp, src/api/main.cpp).
void RegisterThematicTrimmingVariables();
} // namespace Antares::Solver::Variable
