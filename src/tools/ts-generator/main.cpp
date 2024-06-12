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

#include <yuni/core/getopt.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/solver/ts-generator/generator.h>
#include <antares/study/header.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include <antares/writer/result_format.h>
#include <antares/writer/writer_factory.h>
#include <antares/solver/ts-generator/law.h>

using namespace Antares;

namespace fs = std::filesystem;

using LinkPair = std::pair<std::string, std::string>;
using LinkPairs = std::vector<LinkPair>;

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

    parser->addFlag(settings.allThermal,
                    ' ',
                    "all-thermal",
                    "Generate TS for all thermal clusters");
    parser->addFlag(settings.thermalListToGen,
                    ' ',
                    "thermal",
                    "Generate TS for a list of area IDs and thermal clusters IDs, "
                    "\nusage: --thermal=\"areaID.clusterID;area2ID.clusterID\"");

    parser->addFlag(settings.allLinks, ' ', "all-links", "Generate TS capacities for all links");
    parser->addFlag(settings.linksListToGen,
                    ' ',
                    "links",
                    "Generate TS capacities for a list of 2 area IDs, "
                    "usage: --links=\"areaID.area2ID;area3ID.area1ID\"");

    parser->remainingArguments(settings.studyFolder);

    return parser;
}

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

TSGenerator::listOfLinks getLinksToGen(Data::AreaList& areas, const std::string& linksToGen)
{
    TSGenerator::listOfLinks links;
    const auto ids = splitStringIntoPairs(linksToGen, ';', '.');

    for (const auto& [areaFromID, areaWithID]: ids)
    {
        logs.info() << "Searching for link: " << areaFromID << "/" << areaWithID;

        auto* link = areas.findLink(areaFromID, areaWithID);
        if (!link)
        {
            logs.warning() << "Link not found: " << areaFromID << "/" << areaWithID;
            continue;
        }

        links.emplace_back(link);
    }

    return links;
}

// =====  New code for TS generation links ====================================

std::vector<std::string> extractTargetAreas(fs::path sourceLinkDir)
{
    std::vector<std::string> to_return;
    fs::path pathToIni = sourceLinkDir / "properties.ini";
    IniFile ini;
    ini.open(pathToIni); // gp : we should handle reading issues
    for (auto* s = ini.firstSection; s; s = s->next)
    {
        std::string targetAreaName = transformNameIntoID(s->name);
        to_return.push_back(targetAreaName);
    }
    return to_return;
}

LinkPairs extractLinkNamesFromStudy(fs::path studyDir)
{
    LinkPairs to_return;
    fs::path linksDir = studyDir / "input" / "links";
    for (auto const& item : fs::directory_iterator{linksDir})
    {
        if (item.is_directory())
        {
            std::string sourceAreaName = item.path().filename().generic_string();
            auto targetAreas = extractTargetAreas(item);
            for (auto& targetAreaName : targetAreas)
            {
                auto linkPair = std::make_pair(sourceAreaName, targetAreaName);
                to_return.push_back(linkPair);
            }
        }
    }
    return to_return;
}

bool pairs_match(const LinkPair& p1, const LinkPair& p2)
{
    return (p1.first == p2.first && p1.second == p2.second)
            || (p1.first == p2.second && p1.second == p2.first);
}

const LinkPair* getMatchingPairInCollection(const LinkPair& pair, const LinkPairs& collection)
{
    for(const auto& p : collection)
    {
        if (pairs_match(pair, p))
            return &p;
    }
    return nullptr;
}

LinkPairs extractLinkNamesFromCmdLine(const LinkPairs& allLinks,
                                  const std::string linksFromCmdLine)
{
    LinkPairs to_return;
    LinkPairs pairsFromCmdLine = splitStringIntoPairs(linksFromCmdLine, ';', '.');
    for (auto& p : pairsFromCmdLine)
    {
        if (const auto* found_pair = getMatchingPairInCollection(p, allLinks); found_pair)
        {
            to_return.push_back(*found_pair);
        }
        else
        {
            logs.error() << "Link '" << p.first << "." << p.second << "' not found";
        }
    }
    return to_return;
}

void logLinks(std::string title, LinkPairs& links)
{
    std::cout << title << " : " << std::endl;
    for (auto& link : links)
    {
        std::cout << "+ " << link.first << "." << link.second << std::endl;
    }
}

struct StudyParamsForLinkTS
{
    unsigned int nbLinkTStoGenerate = 1;
    // gp : we will have a problem with that if seed-tsgen-links not set in
    // gp : generaldata.ini. In that case, our default value is wrong.
    unsigned int seed = Data::antaresSeedDefaultValue;
};

bool readLinkGeneralProperty(StudyParamsForLinkTS& params,
                             const Yuni::String& key,
                             const Yuni::String& value)
{
    if (key == "nbtimeserieslinks")
    {
        return value.to<unsigned int>(params.nbLinkTStoGenerate);
    }
    if (key == "seed-tsgen-links")
    {
        return value.to<unsigned int>(params.seed);
    }
    return true; // gp : should we return true here ?
}

StudyParamsForLinkTS readGeneralParamsForLinksTS(fs::path studyDir)
{
    StudyParamsForLinkTS to_return;
    fs::path pathToGeneraldata = studyDir / "settings" / "generaldata.ini";
    IniFile ini;
    ini.open(pathToGeneraldata); // gp : we should handle reading issues
    for (auto* section = ini.firstSection; section; section = section->next)
    {
        // Skipping sections useless in the current context
        Yuni::String sectionName = section->name;
        if (sectionName != "general" && sectionName != "seeds - Mersenne Twister")
            continue;

        for (const IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            if (! readLinkGeneralProperty(to_return, p->key, p->value))
            {
                logs.warning() << ini.filename() << ": reading value of '"
                               << p->key << "' went wrong";
            }
        }
    }
    return to_return;
}

struct LinkTSgenerationParams
{
    LinkPair namesPair;
    StudyParamsForLinkTS generalParams;

    unsigned unitCount = 0;
    double nominalCapacity = 0;

    double forcedVolatility = 0.;
    double plannedVolatility = 0.;

    Data::StatisticalLaw forcedLaw = Data::LawUniform;
    Data::StatisticalLaw plannedLaw = Data::LawUniform;

    std::unique_ptr<Data::PreproAvailability> prepro;

    Matrix<> modulationCapacityDirect;
    Matrix<> modulationCapacityIndirect;

    bool forceNoGeneration = false;
    bool hasValidData = true;
};

std::vector<LinkTSgenerationParams> CreateLinkList(const LinkPairs& linksFromCmdLine,
                                                   const StudyParamsForLinkTS& params)
{
    std::vector<LinkTSgenerationParams> to_return(linksFromCmdLine.size());
    unsigned int index = 0;
    for (const auto& link_pair : linksFromCmdLine)
    {
        to_return[index].namesPair = link_pair;
        to_return[index].generalParams = params;
        index++;
    }
    return to_return;
}

LinkTSgenerationParams* findLinkInList(const LinkPair& link_to_find,
                                       std::vector<LinkTSgenerationParams>& linkList)
{
    for(auto& link : linkList)
    {
        if (link.namesPair == link_to_find)
            return &link;
    }
    return nullptr;
}

bool readLinkIniProperty(LinkTSgenerationParams* link,
                         const Yuni::String& key,
                         const Yuni::String& value)
{
    if (key == "unitcount")
    {
        return value.to<uint>(link->unitCount);
    }

    if (key == "nominalcapacity")
    {
        return value.to<double>(link->nominalCapacity);
    }

    if (key == "law.planned")
    {
        return value.to(link->plannedLaw);
    }

    if (key == "law.forced")
    {
        return value.to(link->forcedLaw);
    }

    if (key == "volatility.planned")
    {
        return value.to(link->plannedVolatility);
    }

    if (key == "volatility.forced")
    {
        return value.to(link->forcedVolatility);
    }

    if (key == "force-no-generation")
    {
        return value.to<bool>(link->forceNoGeneration);
    }

    return true;
}

void readLinkIniProperties(LinkTSgenerationParams* link,
                           IniFile::Section* section)
{
    for (const IniFile::Property* p = section->firstProperty; p; p = p->next)
    {
        if (! readLinkIniProperty(link, p->key, p->value))
        {
            std::string linkName = link->namesPair.first + "." + link->namesPair.second;
            logs.warning() << "Link '" << linkName << "' : reading value of '"
                           << p->key << "' went wrong";
        }
    }
}

void readSourceAreaIniFile(fs::path pathToIni,
                           std::string sourceAreaName,
                           std::vector<LinkTSgenerationParams>& linkList)
{
    IniFile ini;
    ini.open(pathToIni); // gp : we should handle reading issues
    for (auto* section = ini.firstSection; section; section = section->next)
    {
        std::string targetAreaName = transformNameIntoID(section->name);
        const LinkPair processedLink = std::make_pair(sourceAreaName, targetAreaName);
        if (auto* foundLink = findLinkInList(processedLink, linkList); foundLink)
        {
            readLinkIniProperties(foundLink, section);
        }
    }
}

void readIniProperties(std::vector<LinkTSgenerationParams>& linkList, fs::path toLinksDir)
{
    for(auto& link : linkList)
    {
        std::string sourceAreaName = link.namesPair.first;
        fs::path pathToIni = toLinksDir / sourceAreaName / "properties.ini";
        readSourceAreaIniFile(pathToIni, sourceAreaName, linkList);
    }
}

bool readLinkPreproTimeSeries(LinkTSgenerationParams& link,
                              fs::path sourceAreaDir)
{
    bool to_return = true;
    const auto preproId = link.namesPair.first + "/" + link.namesPair.second;
    link.prepro = std::make_unique<Data::PreproAvailability>(preproId, link.unitCount);

    auto preproFileRoot = sourceAreaDir / "prepro" / link.namesPair.second;

    auto preproFile = preproFileRoot;
    preproFile += ".txt";
    if (fs::exists(preproFile))
    {
        to_return = link.prepro->data.loadFromCSVFile(
                                              preproFile.string(),
                                              Data::PreproAvailability::preproAvailabilityMax,
                                              DAYS_PER_YEAR)
                    && link.prepro->validate()
                    && to_return;
    }

    auto modulationFileDirect = preproFileRoot;
    modulationFileDirect += "_mod_direct.txt";
    if (fs::exists(modulationFileDirect))
    {
        to_return = link.modulationCapacityDirect.loadFromCSVFile(
                                                modulationFileDirect.string(),
                                                1,
                                                HOURS_PER_YEAR)
                    && to_return;
    }

    auto modulationFileIndirect = preproFileRoot;
    modulationFileIndirect += "_mod_indirect.txt";
    if (fs::exists(modulationFileIndirect))
    {
        to_return = link.modulationCapacityIndirect.loadFromCSVFile(
                                                modulationFileIndirect.string(),
                                                1,
                                                HOURS_PER_YEAR)
                    && to_return;
    }
    // Makes possible a skip of TS generation when time comes
    link.hasValidData = link.hasValidData && to_return;

    return to_return;
}

bool readPreproTimeSeries(std::vector<LinkTSgenerationParams>& linkList,
                          fs::path toLinksDir)
{
    for(auto& link : linkList)
    {
        std::string sourceAreaName = link.namesPair.first;
        fs::path sourceAreaDir = toLinksDir / sourceAreaName;
        if (! readLinkPreproTimeSeries(link, sourceAreaDir))
        {
            logs.error() << "Could not load all prepro data for links '"
                         << link.namesPair.first << "." << link.namesPair.second << "'";
            return false;
        }
    }
    return true;
}

bool readLinksSpecificTSparameters(std::vector<LinkTSgenerationParams>& linkList,
                                   fs::path studyFolder)
{
    fs::path toLinksDir = studyFolder / "input" / "links";
    readIniProperties(linkList, toLinksDir);
    if (! readPreproTimeSeries(linkList, toLinksDir))
    {
        // gp : Log a problem here
        return false;
    }
    return true;
}
// ============================================================================

int main(int argc, char* argv[])
{
    logs.applicationName("ts-generator");

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

    if (settings.allLinks && !settings.linksListToGen.empty())
    {
        logs.error() << "Conflicting options, either choose all links or a list";
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
    const auto linksSavePath = fs::path("ts-generator") / "links";

    // THERMAL
    std::vector<Data::ThermalCluster*> clusters;
    if (settings.allThermal)
    {
        clusters = TSGenerator::getAllClustersToGen(study->areas, true);
    }
    else if (!settings.thermalListToGen.empty())
    {
        clusters = getClustersToGen(study->areas, settings.thermalListToGen);
    }

    for (auto& c: clusters)
    {
        logs.debug() << c->id();
    }

    // LINKS
    // =====  New code for TS generation links ====================================
    auto allLinksPairs = extractLinkNamesFromStudy(settings.studyFolder);
    // logLinks("All links", allLinksPairs);

    auto linksFromCmdLine = extractLinkNamesFromCmdLine(allLinksPairs,
                                                                 settings.linksListToGen);
    // logLinks("Links from cmd line", linksFromCmdLine);
    if (settings.allLinks)
        linksFromCmdLine = allLinksPairs;

    StudyParamsForLinkTS params = readGeneralParamsForLinksTS(settings.studyFolder);

    std::vector<LinkTSgenerationParams> linkList = CreateLinkList(linksFromCmdLine, params);
    if (! readLinksSpecificTSparameters(linkList, settings.studyFolder))
    {
        logs.warning() << "All data could not be loaded for links TS generation";
        // gp : let's be more careful about how we handle problems when loading data.
        // gp : - examining to throw / catch exceptions and marking a link as invalid
        // gp :   when catching the exception at the link level
        // gp : - if a link does have all of its data loaded, then we don't generate TS
        // gp !   for this link.
    }

    // ============================================================================

    TSGenerator::listOfLinks links;
    if (settings.allLinks)
    {
        links = TSGenerator::getAllLinksToGen(study->areas);
    }
    else if (!settings.linksListToGen.empty())
    {
        links = getLinksToGen(study->areas, settings.linksListToGen);
    }

    for (auto& l: links)
    {
        logs.debug() << l->getName();
    }

    bool ret = TSGenerator::generateThermalTimeSeries(*study,
                                                      clusters,
                                                      *resultWriter,
                                                      thermalSavePath.string());
    ret = TSGenerator::generateLinkTimeSeries(*study, links, *resultWriter, linksSavePath.string())
          && ret;

    return !ret; // return 0 for success
}