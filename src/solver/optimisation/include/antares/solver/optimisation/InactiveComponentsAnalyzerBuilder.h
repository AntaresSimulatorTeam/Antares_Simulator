// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include "antares/solver/optimisation/InactiveComponentsAnalyzer.h"
#include "antares/study/fwd.h"

namespace Antares::Optimization
{

// Builds a study-wide InactiveComponentsAnalyzer by inspecting, once, the
// whole loaded chronicle of each area's load / ROR / solar / wind / misc-gen
// series (not re-evaluated per week or per MC year, so an object's
// active/inactive status stays stable across the whole simulation table).
// Areas are visited in `study.areas`' index order, matching
// SIM_InitialisationProblemeHebdo's `pays` numbering.
std::shared_ptr<const InactiveComponentsAnalyzer> BuildInactiveComponentsAnalyzer(
  const Data::Study& study);

} // namespace Antares::Optimization
