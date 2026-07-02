// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <optional>
#include <string>

#include "antares/solver/optimisation/HydroOutputsContext.h"
#include "antares/solver/optimisation/LinksOutputsContext.h"
#include "antares/solver/optimisation/ThermalOutputsContext.h"

// Forward-declared so the constructor below can take a PROBLEME_HEBDO without
// dragging the full simulation problem into this header: the unit tests build a
// minimal context by setting the public members directly (see the note below).
struct PROBLEME_HEBDO;

namespace Antares::Optimization
{

// Result of a per-hour context lookup that every known component (area or
// link) is expected to satisfy. A miss means the context and the solved
// problem have drifted apart; LegacyExtraOutputsContext logs that as a
// warning itself, so callers only decide whether to skip the row, they must
// not warn again.
class RequiredHourlyValue
{
public:
    RequiredHourlyValue() = default;

    RequiredHourlyValue(std::nullopt_t): value_(std::nullopt)
    {
    }

    RequiredHourlyValue(std::optional<double> value): value_(value)
    {
    }

    explicit operator bool() const noexcept
    {
        return value_.has_value();
    }

    double value() const
    {
        return *value_;
    }

private:
    std::optional<double> value_;
};

// Result of a per-hour context lookup whose absence is legitimate by design
// for some components (e.g. inflows, only carried for areas with a
// reservoir). A miss here is an expected, silent case: no warning is logged.
class OptionalHourlyValue
{
public:
    OptionalHourlyValue() = default;

    OptionalHourlyValue(std::optional<double> value): value_(value)
    {
    }

    explicit operator bool() const noexcept
    {
        return value_.has_value();
    }

    double value() const
    {
        return *value_;
    }

private:
    std::optional<double> value_;
};

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

    // Per-hour lookups on the sub-contexts below, keyed by component (area or
    // link name) and absolute time index. Each converts timeIndex to
    // hourInWeek via weekFirstTimeStep. The return type says whether a miss
    // is expected: RequiredHourlyValue is already warned about internally,
    // OptionalHourlyValue is not.
    RequiredHourlyValue load(const std::string& component, unsigned timeIndex) const;
    OptionalHourlyValue inflows(const std::string& component, unsigned timeIndex) const;
    RequiredHourlyValue loopFlow(const std::string& component, unsigned timeIndex) const;
    RequiredHourlyValue directCapacity(const std::string& component, unsigned timeIndex) const;
    RequiredHourlyValue indirectCapacity(const std::string& component, unsigned timeIndex) const;

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
