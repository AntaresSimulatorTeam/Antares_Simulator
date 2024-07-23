/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

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

AdequacyPatchRuntimeData::AdequacyPatchRuntimeData(
  const Antares::Data::AreaList& areas,
  const std::vector<Antares::Data::AreaLink*>& links)
{
    csrTriggeredHoursPerArea_.resize(areas.size());
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
