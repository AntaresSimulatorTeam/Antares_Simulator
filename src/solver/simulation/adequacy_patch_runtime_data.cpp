#include "adequacy_patch_runtime_data.h"

bool AdequacyPatchRuntimeData::wasCSRTriggeredAtAreaHour(int area, int hour) const
{
    return csrTriggered[area].count(hour) > 0;
}

void AdequacyPatchRuntimeData::addCSRTriggeredAtAreaHour(int area, int hour)
{
    csrTriggered[area].insert(hour);
}

void AdequacyPatchRuntimeData::initialize(Antares::Data::Study& study)
{
    csrTriggered.resize(study.areas.size());
    for (uint i = 0; i != study.areas.size(); ++i)
    {
        const auto& area = *(study.areas[i]);
        areaMode.push_back(area.adequacyPatchMode);
    }
    for (uint i = 0; i < study.runtime->interconnectionsCount; ++i)
    {
        const auto& link = *(study.runtime->areaLink[i]);
        originAreaMode.push_back(link.from->adequacyPatchMode);
        extremityAreaMode.push_back(link.with->adequacyPatchMode);
    }
}
