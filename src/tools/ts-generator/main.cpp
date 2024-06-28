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
            link->hasValidData = false;
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

void readPreproTimeSeries(std::vector<LinkTSgenerationParams>& linkList,
                          fs::path toLinksDir)
{
    for(auto& link : linkList)
    {
        std::string sourceAreaName = link.namesPair.first;
        fs::path sourceAreaDir = toLinksDir / sourceAreaName;
        if (! readLinkPreproTimeSeries(link, sourceAreaDir))
        {
            logs.warning() << "Could not load all prepro data for link '"
                         << link.namesPair.first << "." << link.namesPair.second << "'";
        }
    }
}

void readLinksSpecificTSparameters(std::vector<LinkTSgenerationParams>& linkList,
                                   fs::path studyFolder)
{
    fs::path toLinksDir = studyFolder / "input" / "links";
    readIniProperties(linkList, toLinksDir);
    readPreproTimeSeries(linkList, toLinksDir);
}

std::string DateAndTime()
{
    YString to_return;
    unsigned int now = Yuni::DateTime::Now();
    Yuni::DateTime::TimestampToString(to_return, "%Y%m%d-%H%M", now);
    return to_return.to<std::string>();
}
// ============================================================================

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

    auto thermalSavePath = fs::path(settings.studyFolder) / "output" / FormattedTime("%Y%m%d-%H%M");
    thermalSavePath /= "ts-generator";
    thermalSavePath /= "thermal";

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
                                                      thermalSavePath.string());

    ret = linksTSgenerator.generate() && ret;

    return !ret; // return 0 for success
}