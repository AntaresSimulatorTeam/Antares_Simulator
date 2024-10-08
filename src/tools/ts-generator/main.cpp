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

#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/generator.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/tools/ts-generator/linksTSgenerator.h"
#include "antares/tools/ts-generator/tsGenerationOptions.h"

using namespace Antares::TSGenerator;

namespace fs = std::filesystem;

std::vector<Data::ThermalCluster*> getClustersToGen(Data::AreaList& areas,
                                                    const std::string& clustersToGen)
{
    std::vector<Data::ThermalCluster*> clusters;
    const auto pairsAreaCluster = splitStringIntoPairs(clustersToGen, ';', '.');

    for (const auto& [areaID, clusterID]: pairsAreaCluster)
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

int main(int argc, const char* argv[])
{
    logs.applicationName("ts-generator");

    Settings settings;
    if (!parseOptions(argc, argv, settings))
    {
        return 1;
    }

    if (!checkOptions(settings))
    {
        return 1;
    }

    bool return_code{true};

    if (thermalTSrequired(settings))
    {
        // === Data for TS generation ===
        auto study = std::make_shared<Data::Study>(true);
        Data::StudyLoadOptions studyOptions;
        if (!study->loadFromFolder(settings.studyFolder, studyOptions))
        {
            logs.error() << "Invalid study given to the generator";
            return 1;
        }

        std::vector<Data::ThermalCluster*> clusters;
        if (settings.allThermal)
        {
            clusters = getAllClustersToGen(study->areas, true);
        }
        else if (!settings.thermalListToGen.empty())
        {
            clusters = getClustersToGen(study->areas, settings.thermalListToGen);
        }

        // === TS generation ===
        MersenneTwister thermalRandom;
        thermalRandom.reset(study->parameters.seed[Data::seedTsGenThermal]);
        return_code = TSGenerator::generateThermalTimeSeries(*study, clusters, thermalRandom);

        // === Writing generated TS on disk ===
        auto thermalSavePath = fs::path(settings.studyFolder) / "output"
                               / formatTime(getCurrentTime(), "%Y%m%d-%H%M");
        thermalSavePath /= "ts-generator";
        thermalSavePath /= "thermal";
        writeThermalTimeSeries(clusters, thermalSavePath);
    }

    if (linkTSrequired(settings))
    {
        LinksTSgenerator linksTSgenerator(settings);
        linksTSgenerator.extractData();
        return_code = linksTSgenerator.generate() && return_code;
    }

    return !return_code; // return 0 for success
}
