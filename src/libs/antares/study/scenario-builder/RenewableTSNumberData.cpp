//
// Created by marechaljas on 03/07/23.
//

#include "RenewableTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
void renewableTSNumberData::set(const Antares::Data::RenewableCluster* cluster,
                                const uint year,
                                uint value)
{
    assert(cluster != nullptr);
    if (year < pTSNumberRules.height && cluster->areaWideIndex < pTSNumberRules.width)
        pTSNumberRules[cluster->areaWideIndex][year] = value;
}

bool renewableTSNumberData::apply(Study& study)
{
    bool ret = true;
    CString<512, false> logprefix;
    // Errors
    uint errors = 0;

    // Alias to the current area
    assert(pArea != nullptr);
    assert(pArea->index < study.areas.size());
    Area& area = *(study.areas.byIndex[pArea->index]);
    // The total number of clusters for the area
    // WARNING: We may have some renewable clusters with the `mustrun` option
    auto clusterCount = (uint)area.renewable.clusterCount();

    const uint tsGenCountRenewable = get_tsGenCount(study);

    for (uint clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
    {
        auto& cluster = *(area.renewable.clusters[clusterIndex]);
        // alias to the current column
        assert(clusterIndex < pTSNumberRules.width);
        const auto& col = pTSNumberRules[clusterIndex];

        logprefix.clear() << "Renewable: Area '" << area.name << "', cluster: '" << cluster.name()
                          << "': ";
        ret = ApplyToMatrix(errors, logprefix, *cluster.series, col, tsGenCountRenewable) && ret;
    }
    return ret;
}

uint renewableTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    bool tsGenRenewable = (0 != (parameters.timeSeriesToGenerate & timeSeriesRenewable));
    return tsGenRenewable ? 1 : 0u;
}

void renewableTSNumberData::saveToINIFile(const Study& /* study */,
                                          Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
        return;

// Foreach year
#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->renewable.list.size());
    }
#endif

    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // Foreach renewable cluster...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = get(pArea->renewable.list.byIndex[index], y);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << y << ','
                 << pArea->renewable.list.byIndex[index]->id() << " = " << val << '\n';
        }
    }
}

bool renewableTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // If an area is available, it can only be an overlay for renewable timeseries
    // WARNING: The total number of clusters may vary if used from the
    //   solver or not.
    // WARNING: At this point in time, the variable pArea->renewable.clusterCount()
    //   might not be valid (because not really initialized yet)
    const uint clusterCount = pArea->renewable.list.size();
    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}
}