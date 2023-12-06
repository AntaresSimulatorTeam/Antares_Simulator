#include "generator.h"

namespace Antares::Solver::TSGenerator
{

void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params)
{
    for (auto i = areas.begin(); i != areas.end(); ++i)
    {
        auto& area = *(i->second);

        // Load
        if (params.timeSeriesToRefresh & Data::timeSeriesLoad)
        {
            area.load.series.timeSeries.reset(params.nbTimeSeriesLoad, HOURS_PER_YEAR);
        }

        // Wind
        if (params.timeSeriesToRefresh & Data::timeSeriesWind)
        {
            area.wind.series.timeSeries.reset(params.nbTimeSeriesWind, HOURS_PER_YEAR);
        }

        // Solar
        if (params.timeSeriesToRefresh & Data::timeSeriesSolar)
        {
            area.solar.series.timeSeries.reset(params.nbTimeSeriesSolar, HOURS_PER_YEAR);
        }

        // Hydro
        if (params.timeSeriesToRefresh & Data::timeSeriesHydro)
        {
            area.hydro.series->resize_ROR_STORAGE_MINGEN_whenGeneratedTS(params.nbTimeSeriesHydro);
        }

        // Thermal
        auto end = area.thermal.list.mapping.end();
        for (auto it = area.thermal.list.mapping.begin(); it != end; ++it)
        {
            auto& cluster = *(it->second);
            bool globalThermalTSgeneration = params.timeSeriesToRefresh & Data::timeSeriesThermal;
            if (cluster.doWeGenerateTS(globalThermalTSgeneration))
                cluster.series.timeSeries.reset(params.nbTimeSeriesThermal, HOURS_PER_YEAR);
        }
    }
}

void DestroyAll(Data::Study& study)
{
    Destroy<Data::timeSeriesLoad>(study, (uint)-1);
    Destroy<Data::timeSeriesSolar>(study, (uint)-1);
    Destroy<Data::timeSeriesWind>(study, (uint)-1);
    Destroy<Data::timeSeriesHydro>(study, (uint)-1);
    Destroy<Data::timeSeriesThermal>(study, (uint)-1);
}

} // Antares::Solver::TSGenerator