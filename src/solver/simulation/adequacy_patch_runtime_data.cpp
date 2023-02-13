#include "adequacy_patch_runtime_data.h"

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
    for (uint i = 0; i != areas.size(); ++i)
    {
        areaMode.push_back(areas[i]->adequacyPatchMode);
    }
    for (uint i = 0; i < links.size(); ++i)
    {
        originAreaMode.push_back(links[i]->from->adequacyPatchMode);
        extremityAreaMode.push_back(links[i]->with->adequacyPatchMode);
    }
}
