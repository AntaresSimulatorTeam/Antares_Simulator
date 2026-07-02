// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/LegacyExtraOutputsContext.h"

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

namespace Antares::Optimization
{

namespace
{
std::optional<double> valueAtHour(const std::unordered_map<std::string, std::vector<double>>& byKey,
                                   const std::string& key,
                                   unsigned timeIndex,
                                   unsigned weekFirstTimeStep)
{
    const auto it = byKey.find(key);
    if (it == byKey.end())
    {
        return std::nullopt;
    }
    const unsigned pdt = timeIndex - weekFirstTimeStep;
    if (pdt >= it->second.size())
    {
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
    return valueAtHour(hydro.loadByArea, component, timeIndex, weekFirstTimeStep);
}

std::optional<double> LegacyExtraOutputsContext::inflows(const std::string& component,
                                                           unsigned timeIndex) const
{
    return valueAtHour(hydro.inflowsByArea, component, timeIndex, weekFirstTimeStep);
}

std::optional<double> LegacyExtraOutputsContext::loopFlow(const std::string& component,
                                                            unsigned timeIndex) const
{
    return valueAtHour(links.loopFlowByLink, component, timeIndex, weekFirstTimeStep);
}

std::optional<double> LegacyExtraOutputsContext::directCapacity(const std::string& component,
                                                                  unsigned timeIndex) const
{
    return valueAtHour(links.directCapacityByLink, component, timeIndex, weekFirstTimeStep);
}

std::optional<double> LegacyExtraOutputsContext::indirectCapacity(const std::string& component,
                                                                    unsigned timeIndex) const
{
    return valueAtHour(links.indirectCapacityByLink, component, timeIndex, weekFirstTimeStep);
}

} // namespace Antares::Optimization
