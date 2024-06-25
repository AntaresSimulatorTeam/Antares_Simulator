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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include <filesystem>
#include <memory>
#include <string>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/generator.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include <antares/writer/result_format.h>
#include <antares/writer/writer_factory.h>

#include "antares/tools/ts-generator/tsGenerationOptions.h"
#include "antares/tools/ts-generator/linksTSgenerator.h"

using namespace Antares::TSGenerator;

namespace fs = std::filesystem;

std::vector<Data::ThermalCluster*> getClustersToGen(Data::AreaList& areas,
                                                    const std::string& clustersToGen)
{
    std::vector<Data::ThermalCluster*> clusters;
    const auto ids = splitStringIntoPairs(clustersToGen, ';', '.');

    for (const auto& [areaID, clusterID]: ids)
    {
        logs.info() << "Searching for area: " << areaID << " and cluster: " << clusterID;

        auto* area = areas.find(areaID);
        if (!area)
        {
            logs.warning() << "Area not found: " << areaID;
            continue;
        }

        auto* cluster = area->thermal.list.findInAll(clusterID);
        if (!cluster)
        {
            logs.warning() << "Cluster not found: " << clusterID;
            continue;
        }

        clusters.push_back(cluster);
    }

    return clusters;
}

int main(int argc, char* argv[])
{
    logs.applicationName("ts-generator");

    Settings settings;
    if (! parseOptions(argc, argv, settings))
        return 1;

    if (! checkOptions(settings))
        return 1;

    auto study = std::make_shared<Data::Study>(true);
    Data::StudyLoadOptions studyOptions;
    studyOptions.prepareOutput = true;

    if (!study->loadFromFolder(settings.studyFolder, studyOptions))
    {
        logs.error() << "Invalid study given to the generator";
        return 1;
    }

    study->initializeRuntimeInfos();
    // Force the writing of generated TS into output/YYYYMMDD-HHSSeco/ts-generator/thermal[/mc-0]
    study->parameters.timeSeriesToArchive |= Antares::Data::timeSeriesThermal;

    try
    {
        Antares::Check::checkMinStablePower(true, study->areas);
    }
    catch (Error::InvalidParametersForThermalClusters& ex)
    {
        Antares::logs.error() << ex.what();
    }

    Benchmarking::DurationCollector durationCollector;

    auto resultWriter = Solver::resultWriterFactory(Data::ResultFormat::legacyFilesDirectories,
                                                    study->folderOutput,
                                                    nullptr,
                                                    durationCollector);

    const auto thermalSavePath = fs::path("ts-generator") / "thermal";

    // ============ THERMAL : Getting data for generating time-series =========
    std::vector<Data::ThermalCluster*> clusters;
    if (settings.allThermal)
    {
        clusters = getAllClustersToGen(study->areas, true);
    }
    else if (!settings.thermalListToGen.empty())
    {
        clusters = getClustersToGen(study->areas, settings.thermalListToGen);
    }

    for (auto& c: clusters)
    {
        logs.debug() << c->id();
    }



    LinksTSgenerator linksTSgenerator(settings);
    linksTSgenerator.extractData();

    // ============ TS Generation =============================================

    bool ret = TSGenerator::generateThermalTimeSeries(*study,
                                                      clusters,
                                                      *resultWriter,
                                                      thermalSavePath.string());

    ret = linksTSgenerator.generate() && ret;

    return !ret; // return 0 for success
}