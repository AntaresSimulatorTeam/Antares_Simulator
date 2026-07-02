// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/solver/optimisation/HydroOutputsContext.h"
#include "antares/solver/optimisation/LinksOutputsContext.h"
#include "antares/solver/optimisation/ThermalOutputsContext.h"

// Forward-declared so the constructor below can take a PROBLEME_HEBDO without
// dragging the full simulation problem into this header: the unit tests build a
// minimal context by setting the public members directly (see the note below).
struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Study-data lookups the legacy extra outputs need when the formula uses a
// parameter that is NOT an objective coefficient on a recorded variable (and
// therefore cannot be read through LegacySolutionView::linearCost).
//
// A thin composition of three per-domain sub-contexts (see each one's own
// header comment for its data and lookup keys):
//   - hydro:   HydroOutputsContext  (reservoir capacity, inflows, load)
//   - links:   LinksOutputsContext  (transmission capacities, loop flow)
//   - thermal: ThermalOutputsContext (per-cluster emissions and margin data)
//
// The struct is populated by the caller from PROBLEME_HEBDO once per
// FillLegacySimulationTable call and consumed by AddLegacyExtraOutputs. It
// decouples the extra-output code from PROBLEME_HEBDO so the unit tests can
// build a minimal context without dragging the full problem in.
struct LegacyExtraOutputsContext
{
    // Empty context (used by unit tests and the no-table path).
    LegacyExtraOutputsContext() = default;

    // Snapshot the week-wide study data the legacy extra outputs need but
    // cannot read off the problem's variables, by delegating to each
    // sub-context's own constructor. Built once per week from problemeHebdo:
    // the snapshotted data is week-wide and constant across the week's
    // daily/weekly blocks.
    explicit LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo);

    HydroOutputsContext hydro;
    LinksOutputsContext links;
    ThermalOutputsContext thermal;

    // Absolute time index of the week's first hour (PROBLEME_HEBDO::HeureDansLAnnee,
    // i.e. weekInTheYear * hoursPerWeek). LegacyVariableInfo::timeIndex is
    // absolute, so hourInWeek = timeIndex - weekFirstTimeStep indexes the
    // per-hour vectors carried by the sub-contexts above.
    unsigned weekFirstTimeStep = 0;
};

} // namespace Antares::Optimization
