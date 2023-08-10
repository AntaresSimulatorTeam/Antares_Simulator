//
// Created by marechaljas on 04/07/23.
//

#include "ThermalTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool thermalTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    assert(pArea != nullptr);

    // If an area is available, it can only be an overlay for thermal timeseries
    // WARNING: The total number of clusters may vary if used from the
    //   solver or not.
    // WARNING: At this point in time, the variable pArea->thermal.clusterCount()
    //   might not be valid (because not really initialized yet)
    uint clusterCount = (study.usedByTheSolver)
                        ? (pArea->thermal.list.size() + pArea->thermal.mustrunList.size())
                        : pArea->thermal.list.size();

    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}

void thermalTSNumberData::saveToINIFile(const Study& /* study */,
                                        Yuni::IO::File::Stream& file) const
{
    // Prefix
    CString<512, false> prefix;
    prefix += get_prefix();

    if (!pArea)
        return;

#ifndef NDEBUG
    if (pTSNumberRules.width)
    {
        assert(pTSNumberRules.width == pArea->thermal.list.size());
    }
#endif

    // Foreach thermal cluster...
    for (uint index = 0; index != pTSNumberRules.width; ++index)
    {
        // Foreach year ...
        for (uint y = 0; y != pTSNumberRules.height; ++y)
        {
            const uint val = get(pArea->thermal.list.byIndex[index], y);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << y << ','
                 << pArea->thermal.list.byIndex[index]->id() << " = " << val << '\n';
        }
    }
}

void thermalTSNumberData::set(const Antares::Data::ThermalCluster* cluster,
                              const uint year,
                              uint value)
{
    assert(cluster != nullptr);
    if (year < pTSNumberRules.height && cluster->areaWideIndex < pTSNumberRules.width)
        pTSNumberRules[cluster->areaWideIndex][year] = value;
}

bool thermalTSNumberData::apply(Study& study)
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
    // WARNING: We may have some thermal clusters with the `mustrun` option
    auto clusterCount = (uint)area.thermal.clusterCount();

    const uint tsGenCountThermal = get_tsGenCount(study);

    for (uint clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
    {
        auto& cluster = *(area.thermal.clusters[clusterIndex]);
        // alias to the current column
        assert(clusterIndex < pTSNumberRules.width);
        const auto& col = pTSNumberRules[clusterIndex];

        logprefix.clear() << "Thermal: Area '" << area.name << "', cluster: '" << cluster.name()
                          << "': ";
        ret = ApplyToMatrix(errors, logprefix, *cluster.series, col, tsGenCountThermal) && ret;
    }
    return ret;
}

uint thermalTSNumberData::get_tsGenCount(const Study& study) const
{
    // General data
    auto& parameters = study.parameters;

    bool tsGenThermal = (0 != (parameters.timeSeriesToGenerate & timeSeriesThermal));
    return tsGenThermal ? parameters.nbTimeSeriesThermal : 0u;
}
}