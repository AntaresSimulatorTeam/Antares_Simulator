// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

#include <antares/antares/constants.h>

namespace
{
constexpr double thresholdForCostCoefficient = 1.e-12;

double computeHurdleCostCoefficient(double unsuppliedEnergyCost1, double unsuppliedEnergyCost2)
{
    double m = std::max(unsuppliedEnergyCost1, unsuppliedEnergyCost2);
    if (std::fabs(m) < thresholdForCostCoefficient)
    {
        m = thresholdForCostCoefficient;
    }
    return 1. / m;
}
} // namespace

bool AdequacyPatchRuntimeData::wasCSRTriggeredAtAreaHour(int area, int hour) const
{
    return csrTriggeredHoursPerArea_[area].count(hour) > 0;
}

void AdequacyPatchRuntimeData::addCSRTriggeredAtAreaHour(int area, int hour)
{
    csrTriggeredHoursPerArea_[area].insert(hour);
}

void AdequacyPatchRuntimeData::resetCSRTriggeredHours()
{
    for (auto& triggeredHours: csrTriggeredHoursPerArea_)
    {
        triggeredHours.clear();
    }
}

bool AdequacyPatchRuntimeData::wasENSZeroedByThresholdAtAreaHour(int area, int hour) const
{
    return ensZeroedByThresholdPerArea_[area].count(hour) > 0;
}

void AdequacyPatchRuntimeData::addENSZeroedByThresholdAtAreaHour(int area, int hour)
{
    ensZeroedByThresholdPerArea_[area].insert(hour);
}

void AdequacyPatchRuntimeData::resetENSZeroedByThreshold()
{
    for (auto& zeroedHours: ensZeroedByThresholdPerArea_)
    {
        zeroedHours.clear();
    }
}

void AdequacyPatchRuntimeData::setMarginalCostBeforeAdqPatch(int area,
                                                             int hour,
                                                             double marginalCost)
{
    marginalCostsBeforeAdqPatch_[area][hour] = marginalCost;
}

double AdequacyPatchRuntimeData::marginalCostBeforeAdqPatch(int area, int hour) const
{
    return marginalCostsBeforeAdqPatch_[area][hour];
}

AdequacyPatchRuntimeData::AdequacyPatchRuntimeData(
  const Antares::Data::AreaList& areas,
  const std::vector<Antares::Data::AreaLink*>& links)
{
    csrTriggeredHoursPerArea_.resize(areas.size());
    ensZeroedByThresholdPerArea_.resize(areas.size());
    marginalCostsBeforeAdqPatch_.resize(areas.size(),
                                        std::vector<double>(Antares::Constants::nbHoursInAWeek,
                                                            0.));
    areaMode.resize(areas.size());
    for (uint i = 0; i != areas.size(); ++i)
    {
        areaMode[i] = areas[i]->adequacyPatchMode;
    }

    const auto numberOfLinks = links.size();
    originAreaMode.resize(numberOfLinks);
    extremityAreaMode.resize(numberOfLinks);
    hurdleCostCoefficients.resize(numberOfLinks);
    for (uint i = 0; i < numberOfLinks; ++i)
    {
        auto from = links[i]->from;
        auto with = links[i]->with;
        originAreaMode[i] = from->adequacyPatchMode;
        extremityAreaMode[i] = with->adequacyPatchMode;
        hurdleCostCoefficients[i] = computeHurdleCostCoefficient(
          from->thermal.unsuppliedEnergyCost,
          with->thermal.unsuppliedEnergyCost);
    }
}
