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

#include "antares/study/area/links.h"

#include <array>
#include <filesystem>
#include <limits>

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include "antares/study//study.h"
#include "antares/study/area/area.h"
#include "antares/utils/utils.h"

using namespace Yuni;
using namespace Antares;

namespace fs = std::filesystem;

#define SEP (IO::Separator)

namespace Antares
{
namespace Data
{
AreaLink::AreaLink():
    from(nullptr),
    with(nullptr),
    parameters(fhlMax, HOURS_PER_YEAR),
    directCapacities(timeseriesNumbers),
    indirectCapacities(timeseriesNumbers),
    useLoopFlow(false),
    usePST(false),
    useHurdlesCost(false),
    transmissionCapacities(LocalTransmissionCapacities::enabled),
    assetType(Data::atAC),
    index(0),
    indexForArea(0),
    displayComments(true),
    filterSynthesis(filterAll),
    filterYearByYear(filterAll),
    color{112, 112, 112},
    style(stPlain),
    linkWidth(1)
{
    timeseriesNumbers.registerSeries(&directCapacities, "direct-capacity");
    timeseriesNumbers.registerSeries(&indirectCapacities, "indirect-capacity");

    directCapacities.reset();
    indirectCapacities.reset();
}

AreaLink::~AreaLink()
{
}

bool AreaLink::linkLoadTimeSeries_for_version_below_810(const fs::path& folder)
{
    fs::path path = folder / std::string(with->id + ".txt");

    // Load link's data
    Matrix<> tmpMatrix;
    const uint matrixWidth = 8;
    if (!tmpMatrix.loadFromCSVFile(path.string(),
                                   matrixWidth,
                                   HOURS_PER_YEAR,
                                   Matrix<>::optFixedSize | Matrix<>::optImmediate))
    {
        return false;
    }

    // Store data into link's data container
    for (unsigned int h = 0; h < HOURS_PER_YEAR; h++)
    {
        directCapacities[0][h] = tmpMatrix[0][h];
        indirectCapacities[0][h] = tmpMatrix[1][h];
        parameters[fhlHurdlesCostDirect][h] = tmpMatrix[2][h];
        parameters[fhlHurdlesCostIndirect][h] = tmpMatrix[3][h];
        parameters[fhlImpedances][h] = tmpMatrix[4][h];
        parameters[fhlLoopFlow][h] = tmpMatrix[5][h];
        parameters[fhlPShiftMinus][h] = tmpMatrix[6][h];
        parameters[fhlPShiftPlus][h] = tmpMatrix[7][h];
    }

    return true;
}

bool AreaLink::linkLoadTimeSeries_for_version_820_and_later(const fs::path& folder)
{
    bool success = true;

    // Read link's parameters times series
    std::string paramId = with->id + "_parameters.txt";
    fs::path path = folder / paramId;
    success = parameters.loadFromCSVFile(path.string(),
                                         fhlMax,
                                         HOURS_PER_YEAR,
                                         Matrix<>::optFixedSize)
              && success;

    fs::path capacitiesFolder = folder / "capacities";

    // Read link's direct capacities time series
    path = capacitiesFolder / std::string(with->id + "_direct.txt");
    success = directCapacities.loadFromFile(path, false) && success;

    // Read link's indirect capacities time series
    path = capacitiesFolder / std::string(with->id + "_indirect.txt");
    success = indirectCapacities.loadFromFile(path, false) && success;

    return success;
}

bool AreaLink::isLinkPhysical() const
{
    // All link types are physical, except arVirt
    return assetType != atVirt;
}

// Handle all trivial cases here
void AreaLink::overrideTransmissionCapacityAccordingToGlobalParameter(
  GlobalTransmissionCapacities tncGlobal)
{
    switch (tncGlobal)
    {
    case GlobalTransmissionCapacities::localValuesForAllLinks: // Use the local property for all
                                                               // links, including physical links
        break;
    case GlobalTransmissionCapacities::nullForAllLinks:
        transmissionCapacities = LocalTransmissionCapacities::null;
        break;
    case GlobalTransmissionCapacities::infiniteForAllLinks:
        transmissionCapacities = LocalTransmissionCapacities::infinite;
        break;
    case GlobalTransmissionCapacities::nullForPhysicalLinks: // Use '0' only for physical links
        if (isLinkPhysical())
        {
            transmissionCapacities = LocalTransmissionCapacities::null;
        }
        break;
    case GlobalTransmissionCapacities::infiniteForPhysicalLinks: // Use 'infinity' only for physical
                                                                 // links
        if (isLinkPhysical())
        {
            transmissionCapacities = LocalTransmissionCapacities::infinite;
        }
        break;
    default:
        logs.error() << "Wrong global transmission capacity given to function" << __FUNCTION__;
    }
}

bool AreaLink::loadTimeSeries(const StudyVersion& version, const fs::path& folder)
{
    if (version < StudyVersion(8, 2))
    {
        return linkLoadTimeSeries_for_version_below_810(folder);
    }
    else
    {
        return linkLoadTimeSeries_for_version_820_and_later(folder);
    }
}

void AreaLink::storeTimeseriesNumbers(Solver::IResultWriter& writer) const
{
    std::string filename = with->id + ".txt";
    fs::path path = fs::path("ts-numbers") / "ntc" / from->id.to<std::string>() / filename;

    std::string buffer;
    timeseriesNumbers.saveToBuffer(buffer);
    writer.addEntryFromBuffer(path.string(), buffer);
}

void AreaLink::detach()
{
    assert(from);
    assert(with);

    // Here, we cannot make assumption that the variable `with->id` is up-to-date.
    // Consequently, This code is invalid :
    // this->from->detachLinkFromID(with->id);
    from->detachLinkFromItsPointer(this);
}

void AreaLink::resetToDefaultValues()
{
    parameters.reset(fhlMax, HOURS_PER_YEAR, true);
    directCapacities.reset();
    indirectCapacities.reset();

    for (uint i = 0; i != HOURS_PER_YEAR; ++i)
    {
        directCapacities[0][i] = 1.;
        indirectCapacities[0][i] = 1.;
    }
    useLoopFlow = false;
    usePST = false;
    useHurdlesCost = false;
    transmissionCapacities = LocalTransmissionCapacities::enabled;
    assetType = Data::atAC;
    color[0] = 112;
    color[1] = 112;
    color[2] = 112;
    style = stPlain;

    filterSynthesis = (uint)filterAll;
    filterYearByYear = (uint)filterAll;

    comments.clear();
    comments.shrink();
    displayComments = true;
}

void AreaLink::reverse()
{
    // Logs
    logs.info() << "Reversing the orientation of the link `" << from->id << "` - `" << with->id
                << '`';

    // Keeping a reference to areas, with a direct inversion
    Area* from = this->with;
    Area* with = this->from;

    this->detach();
    // Reset the pointers
    this->from = from;
    this->with = with;
    // Re-attach the link to its origin area
    from->links[with->id] = this;

    // Updating both areas' links local numbering
    from->buildLinksIndexes();
    with->buildLinksIndexes();

    // Making sure that we have the data
    directCapacities.forceReload(true);
    indirectCapacities.forceReload(true);

    // invert NTC values
    directCapacities.timeSeries.swap(indirectCapacities.timeSeries);

    directCapacities.markAsModified();
    indirectCapacities.markAsModified();
}

bool AreaLink::isVisibleOnLayer(const size_t& layerID) const
{
    if (from && with)
    {
        return from->isVisibleOnLayer(layerID) && with->isVisibleOnLayer(layerID);
    }
    return false;
}

AreaLink* AreaAddLinkBetweenAreas(Area* area, Area* with, bool warning)
{
    /* Asserts */
    assert(area);
    assert(with);

    if (warning && area->id > with->id)
    {
        logs.warning() << "Link: `" << area->id << "` - `" << with->id << "`: Invalid orientation";
    }

    if (area->id == with->id)
    {
        logs.error() << "The area named '" << area->id << "' has a link to itself";
        return nullptr;
    }

    AreaLink* link = new AreaLink();
    link->from = area;
    link->with = with;
    area->links[with->id] = link;
    area->buildLinksIndexes();
    return link;
}

namespace // anonymous
{

bool isPropertyUsedForLinkTSgeneration(const std::string& key)
{
    std::array<std::string, 7> listKeys = {"unitcount",
                                           "nominalcapacity",
                                           "law.planned",
                                           "law.forced",
                                           "volatility.planned",
                                           "volatility.forced",
                                           "force-no-generation"};
    return std::find(listKeys.begin(), listKeys.end(), key) != listKeys.end();
}

bool AreaLinksInternalLoadFromProperty(AreaLink& link, const String& key, const String& value)
{
    if (key == "hurdles-cost")
    {
        return value.to<bool>(link.useHurdlesCost);
    }
    if (key == "loop-flow")
    {
        return value.to<bool>(link.useLoopFlow);
    }
    if (key == "use-phase-shifter")
    {
        return value.to<bool>(link.usePST);
    }
    if (key == "copper-plate")
    {
        bool copperPlate;
        if (value.to<bool>(copperPlate))
        {
            using LocalNTCtype = Data::LocalTransmissionCapacities;
            link.transmissionCapacities = copperPlate ? LocalNTCtype::infinite
                                                      : LocalNTCtype::enabled;
            return true;
        }
        return false;
    }
    if (key == "asset-type")
    {
        if (value == "ac")
        {
            link.assetType = atAC;
        }
        else if (value == "dc")
        {
            link.assetType = atDC;
        }
        else if (value == "gaz")
        {
            link.assetType = atGas;
        }
        else if (value == "virt")
        {
            link.assetType = atVirt;
        }
        else if (value == "other")
        {
            link.assetType = atOther;
        }
        else
        {
            link.assetType = atOther;
            return false;
        }
        return true;
    }
    if (key == "link-style")
    {
        if (value == "plain")
        {
            link.style = stPlain;
        }
        else if (value == "dot")
        {
            link.style = stDot;
        }
        else if (value == "dash")
        {
            link.style = stDash;
        }
        else if (value == "dotdash")
        {
            link.style = stDotDash;
        }
        else
        {
            link.style = stPlain;
            return false;
        }
        return true;
    }

    if (key == "link-width")
    {
        link.linkWidth = std::clamp(value.to<int>(), 1, 6);
        return true;
    }
    if (key == "colorr")
    {
        link.color[0] = std::clamp(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "colorg")
    {
        link.color[1] = std::clamp(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "colorb")
    {
        link.color[2] = std::clamp(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "transmission-capacities")
    {
        using LocalNTCtype = Data::LocalTransmissionCapacities;
        if (value == "enabled")
        {
            link.transmissionCapacities = LocalNTCtype::enabled;
        }
        else if (value == "infinite")
        {
            link.transmissionCapacities = LocalNTCtype::infinite;
        }
        else if (value == "ignore")
        {
            link.transmissionCapacities = LocalNTCtype::null;
        }
        else
        {
            link.transmissionCapacities = LocalNTCtype::null;
            return false;
        }
        return true;
    }

    if (key == "display-comments")
    {
        return value.to<bool>(link.displayComments);
    }
    if (key == "comments")
    {
        link.comments = value;
        return true;
    }
    if (key == "filter-synthesis")
    {
        link.filterSynthesis = stringIntoDatePrecision(value);
        return true;
    }
    if (key == "filter-year-by-year")
    {
        link.filterYearByYear = stringIntoDatePrecision(value);
        return true;
    }
    // Properties used by TS generator only.
    // We just skip them (otherwise : reading error)
    return isPropertyUsedForLinkTSgeneration(key.to<std::string>());
}

[[noreturn]] void logLinkDataCheckError(const AreaLink& link, const String& msg, int hour)
{
    logs.error() << "Link (" << link.from->name << "/" << link.with->name << "): Invalid values ("
                 << msg << ") for hour " << hour;
    throw Antares::Error::ReadingStudy();
}

[[noreturn]] void logLinkDataCheckErrorDirectIndirect(const AreaLink& link,
                                                      uint direct,
                                                      uint indirect)
{
    logs.error() << "Link (" << link.from->name << "/" << link.with->name << "): Found " << direct
                 << " direct TS " << " and " << indirect
                 << " indirect TS, expected the same number";
    throw Antares::Error::ReadingStudy();
}
} // anonymous namespace

bool AreaLinksLoadFromFolder(Study& study, AreaList* areaList, Area* area, const fs::path& folder)
{
    // Assert
    assert(area);

    /* Initialize */
    fs::path path = folder / "properties.ini";

    IniFile ini;
    if (!ini.open(path))
    {
        return false;
    }

    bool ret = true;
    String key;
    String value;

    // Browse all sections
    for (auto* s = ini.firstSection; s; s = s->next)
    {
        // Getting the name of the area
        const std::string targetAreaName = transformNameIntoID(s->name);

        // Trying to find it
        Area* targetArea = AreaListLFind(areaList, targetAreaName.c_str());
        if (!targetArea)
        {
            logs.error() << '`' << s->name << "`: Impossible to find the area";
            continue;
        }
        AreaLink* lnk = AreaAddLinkBetweenAreas(area, targetArea);
        if (!lnk)
        {
            logs.error() << "Impossible to create a link between two areas";
            continue;
        }

        auto& link = *lnk;

        link.comments.clear();
        link.displayComments = true;

        ret = link.loadTimeSeries(study.header.version, folder) && ret;

        // Checks on loaded link's data
        if (study.usedByTheSolver)
        {
            const uint nbDirectTS = link.directCapacities.timeSeries.width;
            const uint nbIndirectTS = link.indirectCapacities.timeSeries.width;
            if (nbDirectTS != nbIndirectTS)
            {
                logLinkDataCheckErrorDirectIndirect(link, nbDirectTS, nbIndirectTS);
            }

            auto& directHurdlesCost = link.parameters[fhlHurdlesCostDirect];
            auto& indirectHurdlesCost = link.parameters[fhlHurdlesCostIndirect];
            auto& loopFlow = link.parameters[fhlLoopFlow];
            auto& PShiftMinus = link.parameters[fhlPShiftMinus];
            auto& PShiftPlus = link.parameters[fhlPShiftPlus];

            for (uint indexTS = 0; indexTS < nbDirectTS; ++indexTS)
            {
                const double* directCapacities = link.directCapacities[indexTS];
                const double* indirectCapacities = link.indirectCapacities[indexTS];

                // Checks on direct capacities
                for (unsigned int h = 0; h < HOURS_PER_YEAR; h++)
                {
                    if (directCapacities[h] < 0.)
                    {
                        logLinkDataCheckError(link, "direct capacity < 0", h);
                    }
                    if (directCapacities[h] < loopFlow[h])
                    {
                        logLinkDataCheckError(link, "direct capacity < loop flow", h);
                    }
                }

                // Checks on indirect capacities
                for (unsigned int h = 0; h < HOURS_PER_YEAR; h++)
                {
                    if (indirectCapacities[h] < 0.)
                    {
                        logLinkDataCheckError(link, "indirect capacitity < 0", h);
                    }
                    if (indirectCapacities[h] + loopFlow[h] < 0)
                    {
                        logLinkDataCheckError(link, "indirect capacity + loop flow < 0", h);
                    }
                }
            }
            // Checks on hurdle costs
            for (unsigned int h = 0; h < HOURS_PER_YEAR; h++)
            {
                if (directHurdlesCost[h] + indirectHurdlesCost[h] < 0)
                {
                    logLinkDataCheckError(link,
                                          "hurdle costs direct + hurdle cost indirect < 0",
                                          h);
                }
            }

            // Checks on P. shift min and max
            for (unsigned int h = 0; h < HOURS_PER_YEAR; h++)
            {
                if (PShiftPlus[h] < PShiftMinus[h])
                {
                    logLinkDataCheckError(link, "phase shift plus < phase shift minus", h);
                }
            }
        }

        // Inifile
        const IniFile::Property* p = s->firstProperty;
        for (; p; p = p->next)
        {
            key = p->key;
            key.toLower();
            value = p->value;
            if (!AreaLinksInternalLoadFromProperty(link, key, value))
            {
                logs.warning() << '`' << p->key << "`: Unknown property";
            }
        }

        // From the solver only
        if (study.usedByTheSolver)
        {
            link.overrideTransmissionCapacityAccordingToGlobalParameter(
              study.parameters.transmissionCapacities);

            if (!link.useHurdlesCost || !study.parameters.include.hurdleCosts)
            {
                link.parameters.columnToZero(Data::fhlHurdlesCostDirect);
                link.parameters.columnToZero(Data::fhlHurdlesCostIndirect);
            }

            switch (link.transmissionCapacities)
            {
            case Data::LocalTransmissionCapacities::enabled:
                break;
            case Data::LocalTransmissionCapacities::null:
            {
                // Ignore transmission capacities
                link.directCapacities.timeSeries.zero();
                link.indirectCapacities.timeSeries.zero();
                break;
            }
            case Data::LocalTransmissionCapacities::infinite:
            {
                // Copper plate mode
                auto infinity = +std::numeric_limits<double>::infinity();
                link.directCapacities.fill(infinity);
                link.indirectCapacities.fill(infinity);
                break;
            }
            default:
                return false;
            }
        }
    }

    return ret;
} // End AreaLinksLoadFromFolder(...)

bool saveAreaLinksTimeSeriesToFolder(const Area* area, const char* const folder)
{
    // Initialize
    String capacitiesFolder;
    capacitiesFolder << folder << SEP << "capacities";
    if (!IO::Directory::Create(capacitiesFolder))
    {
        logs.error() << capacitiesFolder << ": Impossible to create the folder";
        return false;
    }

    String filename;
    bool success = true;

    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);

        // Save parameters : 6 time series
        filename.clear() << folder << SEP << link.with->id << "_parameters.txt";
        success = link.parameters.saveToCSVFile(filename) && success;

        // Save direct capacities time series
        filename.clear() << capacitiesFolder << SEP << link.with->id << "_direct.txt";
        success = link.directCapacities.saveToFile(filename, true) && success;

        // Save indirect capacities time series

        filename.clear() << capacitiesFolder << SEP << link.with->id << "_indirect.txt";
        success = link.indirectCapacities.saveToFile(filename, true) && success;
    }

    return success;
}

bool saveAreaLinksConfigurationFileToFolder(const Area* area, const char* const folder)
{
    String filename;
    IniFile ini;

    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);

        auto* section = ini.addSection(link.with->id);

        section->add("hurdles-cost", link.useHurdlesCost);
        section->add("loop-flow", link.useLoopFlow);
        section->add("use-phase-shifter", link.usePST);
        section->add("transmission-capacities",
                     transmissionCapacitiesToString(link.transmissionCapacities));
        section->add("asset-type", assetTypeToString(link.assetType));
        section->add("link-style", styleToString(link.style));
        section->add("link-width", link.linkWidth);
        section->add("colorr", link.color[0]);
        section->add("colorg", link.color[1]);
        section->add("colorb", link.color[2]);
        section->add("display-comments", link.displayComments);
        if (!link.comments.empty())
        {
            section->add("comments", link.comments);
        }
        section->add("filter-synthesis", datePrecisionIntoString(link.filterSynthesis));
        section->add("filter-year-by-year", datePrecisionIntoString(link.filterYearByYear));
    }

    filename.clear() << folder << SEP << "properties.ini";
    return ini.save(filename);
}

bool AreaLinksSaveToFolder(const Area* area, const char* const folder)
{
    /* Assert */
    assert(area);
    assert(folder);

    // Initialize
    if (!IO::Directory::Create(folder))
    {
        logs.error() << folder << ": Impossible to create the folder";
        return false;
    }

    if (!saveAreaLinksConfigurationFileToFolder(area, folder))
    {
        return false;
    }

    if (!saveAreaLinksTimeSeriesToFolder(area, folder))
    {
        return false;
    }

    return true;
}

void AreaLinkRemove(AreaLink* link)
{
    if (!link)
    {
        return;
    }

    // Asserts
    assert(link->from);
    assert(link->with);

    Area* areaFrom = link->from;
    if (areaFrom && !areaFrom->links.empty())
    {
        areaFrom->detachLinkFromID(link->with->id);
        areaFrom->buildLinksIndexes();
    }

    delete link;
}

uint64_t AreaLink::memoryUsage() const
{
    uint64_t to_return = sizeof(AreaLink);
    to_return += parameters.valuesMemoryUsage();
    to_return += directCapacities.memoryUsage();
    to_return += indirectCapacities.memoryUsage();

    return to_return;
}

bool AreaLink::forceReload(bool reload) const
{
    return parameters.forceReload(reload) && directCapacities.forceReload(reload)
           && indirectCapacities.forceReload(reload);
}

void AreaLink::markAsModified() const
{
    parameters.markAsModified();
    directCapacities.markAsModified();
    indirectCapacities.markAsModified();
}

String AreaLink::getName() const
{
    String s;
    s << from->name << "/" << with->name;
    return s;
}

AreaLink::NamePair AreaLink::getNamePair() const
{
    return NamePair(from->name, with->name);
}

} // namespace Data
} // namespace Antares
