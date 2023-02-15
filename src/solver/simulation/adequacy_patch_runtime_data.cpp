#include "adequacy_patch_runtime_data.h"

namespace
{
constexpr double thresholdForCostCoefficient = 1.e-9;
double computeHurdleCostCoefficient(double a, double b)
{
    double m = std::max(a, b);
    if (std::fabs(m) < thresholdForCostCoefficient)
        m = thresholdForCostCoefficient;
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
          from->thermal.unsuppliedEnergyCost, with->thermal.unsuppliedEnergyCost);
    }
}
