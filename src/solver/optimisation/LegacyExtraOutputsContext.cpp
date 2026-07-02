// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"

#include <antares/logs/logs.h>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

namespace
{
// byKey is built once per week from PROBLEME_HEBDO, covering every area/link
// and the whole week (see HydroOutputsContext / LinksOutputsContext), so a
// missing key or an out-of-range hour normally means the study data and the
// solved problem have drifted apart. warnIfMissingKey is false for series
// that are only carried for a subset of components by design (e.g. inflows,
// carried only for areas with a reservoir), so that legitimate case stays
// silent; an out-of-range hour is unexpected either way and always logged.
std::optional<double> valueAtHour(const std::unordered_map<std::string, std::vector<double>>& byKey,
                                   const std::string& key,
                                   unsigned timeIndex,
                                   unsigned weekFirstTimeStep,
                                   const std::string& label,
                                   bool warnIfMissingKey)
{
    const auto it = byKey.find(key);
    if (it == byKey.end())
    {
        if (warnIfMissingKey)
        {
            logs.warning() << "Extra outputs: no " << label << " data found for '" << key
                           << "', skipping its outputs";
        }
        return std::nullopt;
    }
    const unsigned pdt = timeIndex - weekFirstTimeStep;
    if (pdt >= it->second.size())
    {
        logs.warning() << "Extra outputs: " << label << " data for '" << key
                       << "' does not cover hour " << pdt << " of the week, skipping its outputs";
        return std::nullopt;
    }
    return it->second[pdt];
}
} // namespace

LegacyExtraOutputsContext::LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo):
    hydro(problemeHebdo),
    links(problemeHebdo),
    thermal(problemeHebdo)
{
    weekFirstTimeStep = static_cast<unsigned>(problemeHebdo.HeureDansLAnnee);
}

std::optional<double> LegacyExtraOutputsContext::load(const std::string& component,
                                                        unsigned timeIndex) const
{
    return valueAtHour(hydro.loadByArea, component, timeIndex, weekFirstTimeStep, "load", true);
}

std::optional<double> LegacyExtraOutputsContext::inflows(const std::string& component,
                                                           unsigned timeIndex) const
{
    // Only areas with a reservoir carry an inflows series (see
    // HydroOutputsContext::inflowsByArea): a missing key here is expected,
    // not a bug.
    return valueAtHour(hydro.inflowsByArea, component, timeIndex, weekFirstTimeStep, "inflows",
                       false);
}

std::optional<double> LegacyExtraOutputsContext::loopFlow(const std::string& component,
                                                            unsigned timeIndex) const
{
    return valueAtHour(links.loopFlowByLink, component, timeIndex, weekFirstTimeStep, "loop flow",
                       true);
}

std::optional<double> LegacyExtraOutputsContext::directCapacity(const std::string& component,
                                                                  unsigned timeIndex) const
{
    return valueAtHour(links.directCapacityByLink, component, timeIndex, weekFirstTimeStep,
                       "direct capacity", true);
}

std::optional<double> LegacyExtraOutputsContext::indirectCapacity(const std::string& component,
                                                                    unsigned timeIndex) const
{
    return valueAtHour(links.indirectCapacityByLink, component, timeIndex, weekFirstTimeStep,
                       "indirect capacity", true);
}

} // namespace Antares::Optimization
