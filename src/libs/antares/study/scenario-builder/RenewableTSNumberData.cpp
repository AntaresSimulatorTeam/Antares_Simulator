//
// Created by marechaljas on 03/07/23.
//

#include "RenewableTSNumberData.h"
#include "applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
void renewableTSNumberData::setTSnumber(const Antares::Data::RenewableCluster* cluster,
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

    const uint tsGenCountRenewable = get_tsGenCount(study);

    uint clusterIndex = 0;
    for (const auto& cluster : area.renewable.list)
    {
        // alias to the current column
        assert(clusterIndex < pTSNumberRules.width);
        const auto& col = pTSNumberRules[clusterIndex];

        logprefix.clear() << "Renewable: area '" << area.name << "', cluster: '" << cluster->name()
                          << "': ";
        ret = ApplyToMatrix(errors, logprefix, cluster->series, col, tsGenCountRenewable) && ret;
        clusterIndex++;
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

    for (auto cluster : pArea->renewable.list.all())
    {
        for (uint year = 0; year != pTSNumberRules.height; ++year)
        {
            const uint val = get(cluster.get(), year);
            // Equals to zero means 'auto', which is the default mode
            if (!val)
                continue;
            file << prefix << pArea->id << "," << year << ','
                 << cluster->id() << " = " << val << '\n';
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
    const uint clusterCount = pArea->renewable.list.allClustersCount();
    // Resize
    pTSNumberRules.reset(clusterCount, nbYears);
    return true;
}
}
