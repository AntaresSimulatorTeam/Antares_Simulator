// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/benchmarking/DurationCollector.h>
#include <antares/solver/simulation/regenerate_timeseries.h>
#include <antares/solver/ts-generator/generator.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>

namespace Antares::Solver::Simulation
{
void regenerateTimeSeries(Data::Study& study,
                          IResultWriter& resultWriter,
                          Benchmarking::DurationCollector& durationCollector)
{
    // A preprocessor can be launched for several reasons:
    // * The option "Preprocessor" is checked in the interface _and_ year == 0
    // * Both options "Preprocessor" and "Refresh" are checked in the interface
    //   _and_ the refresh must be done for the given year (always done for the first year).
    using namespace TSGenerator;

    const Data::Parameters& p = study.parameters;
    // Load
    if (Data::timeSeriesLoad & p.timeSeriesToGenerate)
    {
        durationCollector("tsgen_load")
          << [&] { GenerateTimeSeries<Data::timeSeriesLoad>(study, resultWriter); };
    }
    // Solar
    if (Data::timeSeriesSolar & p.timeSeriesToGenerate)
    {
        durationCollector("tsgen_solar")
          << [&] { GenerateTimeSeries<Data::timeSeriesSolar>(study, resultWriter); };
    }
    // Wind
    if (Data::timeSeriesWind & p.timeSeriesToGenerate)
    {
        durationCollector("tsgen_wind")
          << [&] { GenerateTimeSeries<Data::timeSeriesWind>(study, resultWriter); };
    }
    // Hydro
    if (Data::timeSeriesHydro & p.timeSeriesToGenerate)
    {
        durationCollector("tsgen_hydro")
          << [&] { GenerateTimeSeries<Data::timeSeriesHydro>(study, resultWriter); };
    }

    durationCollector("tsgen_thermal") << [&]
    {
        bool globalThermalTSgeneration = study.parameters.timeSeriesToGenerate
                                         & Data::timeSeriesThermal;
        auto clusters = getAllClustersToGen(study.areas, globalThermalTSgeneration);
        generateThermalTimeSeries(study, clusters, study.runtime.random[Data::seedTsGenThermal]);

        bool archive = study.parameters.timeSeriesToArchive & Data::timeSeriesThermal;
        bool doWeWrite = archive && !study.parameters.noOutput;
        if (doWeWrite)
        {
            fs::path savePath = study.folderOutput / "ts-generator" / "thermal" / "mc-" / "0";
            writeThermalTimeSeries(clusters, savePath);
        }

        // apply the spinning if we generated some in memory clusters
        for (auto* cluster: clusters)
        {
            cluster->calculationOfSpinning();
        }
    };
}
} // namespace Antares::Solver::Simulation
