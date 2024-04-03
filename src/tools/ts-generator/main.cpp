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

#include <antares/solver/ts-generator/generator.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/study/header.h>
#include <antares/study/study.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>
#include <yuni/core/getopt.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/writer/writer_factory.h>
#include <antares/writer/result_format.h>
#include <antares/checks/checkLoadedInputData.h>

using namespace Antares;

struct Settings
{
    std::string studyFolder;

    /// generate TS for all clusters if activated
    bool allThermal = false;
    /// generate TS for a list "area.cluster;area2.cluster2;"
    std::string thermalListToGen = "";

    /// generate TS for all links if activated
    bool allLinks = false;
    /// generate TS for a list "area.link;area2.link2;"
    std::string linksListToGen = "";
};

std::unique_ptr<Yuni::GetOpt::Parser> createTsGeneratorParser(Settings& settings)
{
    auto parser = std::make_unique<Yuni::GetOpt::Parser>();
    parser->addParagraph("Antares Time Series generator\n");

    parser->addFlag(settings.allThermal, ' ', "all-thermal", "Generate TS for all thermal clusters");
    parser->addFlag(settings.thermalListToGen, ' ', "thermal", "Generate TS for a list of area IDs and thermal clusters IDs, usage:\n\t--thermal=\"areaID.clusterID;area2ID.clusterID\"");

    parser->addFlag(settings.allLinks, ' ', "all-links", "Generate TS capacities for all links");
    parser->addFlag(settings.linksListToGen, ' ', "links", "Generate TS capacities for a list of area IDs and links name, usage:\n\t--links=\"areaID.area2ID;area3ID.area1ID\"");

    parser->remainingArguments(settings.studyFolder);

    return parser;
}

std::vector<Data::ThermalCluster*> getClustersToGen(Data::AreaList& areas,
                                                    const std::string& clustersToGen)
{
    std::vector<Data::ThermalCluster*> clusters;
    const auto ids = splitStringIntoPairs(clustersToGen, ';', '.');

    for (const auto& [areaID, clusterID] : ids)
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

TSGenerator::listOfLinks getLinksToGen(Data::AreaList& areas,
                                           const std::string& clustersToGen)
{
    TSGenerator::listOfLinks links;
    const auto ids = splitStringIntoPairs(clustersToGen, ';', '.');

    for (const auto& [areaFromID, areaWithID] : ids)
    {
        logs.info() << "Searching for link: " << areaFromID << "/" << areaWithID;

        auto* link = areas.findLink(areaFromID, areaWithID);
        if (!link)
        {
            logs.warning() << "Link not found: " << areaFromID << "/" << areaWithID;
            continue;
        }
        auto direction = (link->from->id == areaFromID) ? TSGenerator::linkDirection::direct :
                    TSGenerator::linkDirection::indirect;

        links.emplace_back(link, direction);
    }

    return links;
}

int main(int argc, char *argv[])
{
    Settings settings;

    auto parser = createTsGeneratorParser(settings);
    switch (auto ret = parser->operator()(argc, argv); ret)
    {
        using namespace Yuni::GetOpt;
    case ReturnCode::error:
        logs.error() << "Unknown arguments, aborting";
        return parser->errors();
    case ReturnCode::help:
        // End the program
        return 0;
    default:
        break;
    }

    if (settings.allThermal && !settings.thermalListToGen.empty())
    {
        logs.error() << "Conflicting options, either choose all thermal clusters or a list";
        return 1;
    }

    auto study = std::make_shared<Data::Study>(true);
    Data::StudyLoadOptions studyOptions;
    studyOptions.prepareOutput = true;
    studyOptions.linksLoadTSGen = true;

    if (!study->loadFromFolder(settings.studyFolder, studyOptions))
    {
        logs.error() << "Invalid study given to the generator";
        return 1;
    }

    study->initializeRuntimeInfos();
    // Force the writing of generated TS into output/YYYYMMDD-HHSSeco/ts-generator/thermal/mc-0
    study->parameters.timeSeriesToArchive |= Antares::Data::timeSeriesThermal;

    try {
        Antares::Check::checkMinStablePower(true, study->areas);
    } catch(Error::InvalidParametersForThermalClusters& ex) {
        Antares::logs.error() << ex.what();
    }

    Benchmarking::NullDurationCollector nullDurationCollector;

    auto resultWriter = Solver::resultWriterFactory(
            Data::ResultFormat::legacyFilesDirectories, study->folderOutput, nullptr, nullDurationCollector);

    const auto thermalSavePath = std::filesystem::path("ts-generator") / "thermal";
    const auto linksSavePath = std::filesystem::path("ts-generator") / "links";

    // THERMAL
    std::vector<Data::ThermalCluster*> clusters;
    if (settings.allThermal)
        clusters = TSGenerator::getAllClustersToGen(study->areas, true);
    else if (!settings.thermalListToGen.empty())
        clusters = getClustersToGen(study->areas, settings.thermalListToGen);

    for (auto& c : clusters)
        logs.debug() << c->id();

    // LINKS
    TSGenerator::listOfLinks links;
    if (settings.allLinks)
        links = TSGenerator::getAllLinksToGen(study->areas);
    else if (!settings.linksListToGen.empty())
        links = getLinksToGen(study->areas, settings.linksListToGen);

    for (auto& l : links)
        logs.debug() << l.first->getName();

    bool ret = TSGenerator::generateThermalTimeSeries(*study, clusters, *resultWriter, thermalSavePath);
    ret = TSGenerator::generateLinkTimeSeries(*study, links, *resultWriter, linksSavePath) && ret;

    return !ret; // return 0 for success
}
