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
// solved problem have drifted apart. An out-of-range hour is always
// unexpected and logged; a missing key is only logged for series where every
// component is expected to have one (see the two callers below).
std::optional<double> valueAtHour(const std::unordered_map<std::string, std::vector<double>>& byKey,
                                   const std::string& key,
                                   unsigned timeIndex,
                                   unsigned weekFirstTimeStep,
                                   const std::string& label)
{
    const auto it = byKey.find(key);
    if (it == byKey.end())
    {
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

// Wraps valueAtHour() for series every known component is expected to carry:
// a missing key is logged here, so the returned RequiredHourlyValue is
// already accounted for and the caller only decides whether to skip the row.
RequiredHourlyValue requiredValueAtHour(
  const std::unordered_map<std::string, std::vector<double>>& byKey,
  const std::string& key,
  unsigned timeIndex,
  unsigned weekFirstTimeStep,
  const std::string& label)
{
    if (!byKey.contains(key))
    {
        logs.warning() << "Extra outputs: no " << label << " data found for '" << key
                       << "', skipping its outputs";
        return std::nullopt;
    }
    return valueAtHour(byKey, key, timeIndex, weekFirstTimeStep, label);
}
} // namespace

LegacyExtraOutputsContext::LegacyExtraOutputsContext(const PROBLEME_HEBDO& problemeHebdo):
    hydro(problemeHebdo),
    links(problemeHebdo),
    thermal(problemeHebdo)
{
    weekFirstTimeStep = static_cast<unsigned>(problemeHebdo.HeureDansLAnnee);
}

RequiredHourlyValue LegacyExtraOutputsContext::load(const std::string& component,
                                                     unsigned timeIndex) const
{
    return requiredValueAtHour(hydro.loadByArea, component, timeIndex, weekFirstTimeStep, "load");
}

OptionalHourlyValue LegacyExtraOutputsContext::inflows(const std::string& component,
                                                        unsigned timeIndex) const
{
    return valueAtHour(hydro.inflowsByArea, component, timeIndex, weekFirstTimeStep, "inflows");
}

RequiredHourlyValue LegacyExtraOutputsContext::loopFlow(const std::string& component,
                                                         unsigned timeIndex) const
{
    return requiredValueAtHour(links.loopFlowByLink, component, timeIndex, weekFirstTimeStep,
                               "loop flow");
}

RequiredHourlyValue LegacyExtraOutputsContext::directCapacity(const std::string& component,
                                                               unsigned timeIndex) const
{
    return requiredValueAtHour(links.directCapacityByLink, component, timeIndex, weekFirstTimeStep,
                               "direct capacity");
}

RequiredHourlyValue LegacyExtraOutputsContext::indirectCapacity(const std::string& component,
                                                                 unsigned timeIndex) const
{
    return requiredValueAtHour(links.indirectCapacityByLink, component, timeIndex,
                               weekFirstTimeStep, "indirect capacity");
}

} // namespace Antares::Optimization
