/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <limits>
#include <yuni/yuni.h>
#include "../study.h"
#include "links.h"
#include "area.h"
#include "../../logs.h"
#include "../memory-usage.h"
#include "../filter.h"
#include "constants.h"
#include "../fwd.h"
#include <antares/study/links/property-links-helper.h>

using namespace Yuni;
using namespace Antares;

namespace // anonymous
{
struct TSNumbersPredicate
{
    uint32 operator()(uint32 value) const
    {
        return value + 1;
    }
};
} // namespace

#define SEP (IO::Separator)

namespace Antares
{
namespace Data
{
AreaLink::AreaLink() :
 from(nullptr),
 with(nullptr),
 parameters(fhlMax, HOURS_PER_YEAR),
 directCapacities(1, HOURS_PER_YEAR),
 indirectCapacities(1, HOURS_PER_YEAR),
 useLoopFlow(false),
 usePST(false),
 useHurdlesCost(false),
 transmissionCapacities(Data::tncEnabled),
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
}

AreaLink::~AreaLink()
{
}

bool AreaLink::linkLoadTimeSeries_for_version_under_320(const AnyString& folder, Study& study)
{
    String buffer;
    bool ret = true;

    // gp : refactor this part : avoid code duplication
    parameters.resize(fhlMax, HOURS_PER_YEAR);
    parameters.zero();
    if (parameters.jit)
    {
        parameters.jit->options |= Matrix<>::optFixedSize;
        parameters.markAsModified();
    }

    directCapacities.resize(1, HOURS_PER_YEAR);
    directCapacities.zero();
    if (directCapacities.jit)
    {
        directCapacities.jit->options |= Matrix<>::optFixedSize;
        directCapacities.markAsModified();
    }

    indirectCapacities.resize(1, HOURS_PER_YEAR);
    indirectCapacities.zero();
    if (indirectCapacities.jit)
    {
        indirectCapacities.jit->options |= Matrix<>::optFixedSize;
        indirectCapacities.markAsModified();
    }

    uint loadOptions = Matrix<>::optFixedSize | Matrix<>::optImmediate;
    // NTC direct
    buffer.clear() << folder << SEP << with->id << SEP << "direct." << study.inputExtension;
    ret = directCapacities.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    directCapacities.markAsModified();

    // NTC indirect
    buffer.clear() << folder << SEP << with->id << SEP << "indirect." << study.inputExtension;
    ret = indirectCapacities.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    indirectCapacities.markAsModified();

    // Impedances
    buffer.clear() << folder << SEP << with->id << SEP << "impedances." << study.inputExtension;
    Matrix<double> tmp;
    ret = tmp.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    parameters.pasteToColumn(fhlImpedances, tmp[0]);
    parameters.markAsModified();

    return ret;
}

bool AreaLink::linkLoadTimeSeries_for_version_from_320_to_630(const AnyString& folder)
{
    String buffer;
    bool ret = true;

    bool enabledModeIsChanged = false;
    if (JIT::enabled)
    {
        JIT::enabled = false; // Allowing to read the area's daily max power
        enabledModeIsChanged = true;
    }

    // Resize link's data container
    parameters.resize(fhlMax, HOURS_PER_YEAR);
    directCapacities.resize(1, HOURS_PER_YEAR);
    indirectCapacities.resize(1, HOURS_PER_YEAR);

    // Initialize link's data container
    parameters.zero();
    directCapacities.zero();
    indirectCapacities.zero();

    // Load link's data
    buffer.clear() << folder << SEP << with->id << ".txt";
    Matrix<> tmpMatrix;
    ret = tmpMatrix.loadFromCSVFile(
            buffer, 5, HOURS_PER_YEAR, Matrix<>::optFixedSize | Matrix<>::optImmediate)
          && ret;

    // Store data into link's data container
    for (int h = 0; h < HOURS_PER_YEAR; h++)
    {
        directCapacities[0][h] = tmpMatrix[0][h];
        indirectCapacities[0][h] = tmpMatrix[1][h];
        parameters[fhlImpedances][h] = tmpMatrix[2][h];
        parameters[fhlHurdlesCostDirect][h] = tmpMatrix[3][h];
        parameters[fhlHurdlesCostIndirect][h] = tmpMatrix[4][h];
    }

    if (enabledModeIsChanged)
        JIT::enabled = true; // Back to the previous loading mode.

    return ret;
}

bool AreaLink::linkLoadTimeSeries_for_version_from_630_to_810(const AnyString& folder)
{
    String buffer;
    buffer.clear() << folder << SEP << with->id << ".txt";

    // Load link's data
    Matrix<> tmpMatrix;
    const uint matrixWidth = 8;
    if (!tmpMatrix.loadFromCSVFile(
          buffer, matrixWidth, HOURS_PER_YEAR, Matrix<>::optFixedSize | Matrix<>::optImmediate))
    {
        return false;
    }

    // Store data into link's data container
    for (int h = 0; h < HOURS_PER_YEAR; h++)
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

bool AreaLink::linkLoadTimeSeries_for_version_820_and_later(const AnyString& folder)
{
    String capacitiesFolder;
    capacitiesFolder << folder << SEP << "capacities";

    String filename;
    bool success = true;

    // Read link's parameters times series
    filename.clear() << folder << SEP << with->id << "_parameters.txt";
    success = parameters.loadFromCSVFile(filename, fhlMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
              && success;

    // Read link's direct capacities time series
    filename.clear() << capacitiesFolder << SEP << with->id << "_direct.txt";
    success = directCapacities.loadFromCSVFile(filename, 1, HOURS_PER_YEAR) && success;

    // Read link's indirect capacities time series
    filename.clear() << capacitiesFolder << SEP << with->id << "_indirect.txt";
    success = indirectCapacities.loadFromCSVFile(filename, 1, HOURS_PER_YEAR) && success;

    return success;
}

static TransmissionCapacities overridePhysical(TransmissionCapacities tncGlobal,
                                               TransmissionCapacities tncLocal)
{
    switch (tncGlobal)
    {
    case tncInfinitePhysical:
        return tncInfinite;
    case tncIgnorePhysical:
        return tncIgnore;
    case tncEnabled:
        return tncLocal;
    default:
        return tncGlobal;
    }
}

static TransmissionCapacities overrideVirtual(TransmissionCapacities tncGlobal,
                                              TransmissionCapacities tncLocal)
{
    switch (tncGlobal)
    {
    case tncInfinitePhysical:
    case tncIgnorePhysical:
    case tncEnabled:
        return tncLocal;
    default:
        return tncGlobal;
    }
}

// Global Optimization override
void AreaLink::overrideTransmissionCapacityAccordingToGlobalParameter(
  TransmissionCapacities tncGlobal)
{
    switch (assetType)
    {
    case atAC:
    case atDC:
    case atGas:
    case atOther:
        transmissionCapacities = overridePhysical(tncGlobal, transmissionCapacities);
        break;
    case atVirt:
        transmissionCapacities = overrideVirtual(tncGlobal, transmissionCapacities);
        break;
    }
}

bool AreaLink::loadTimeSeries(Study& study, const AnyString& folder)
{
    if (study.header.version < 320)
        return linkLoadTimeSeries_for_version_under_320(folder, study);
    else if (study.header.version < 630)
        return linkLoadTimeSeries_for_version_from_320_to_630(folder);
    else if (study.header.version < 820)
        return linkLoadTimeSeries_for_version_from_630_to_810(folder);
    else
    {
        return linkLoadTimeSeries_for_version_820_and_later(folder);
    }
}

bool AreaLink::storeTimeseriesNumbers(const AnyString& folder) const
{
    TSNumbersPredicate predicate;
    YString filename;
    filename << folder << SEP << with->id << ".txt";
    return timeseriesNumbers.saveToCSVFile(filename, 0, true, predicate);
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
    directCapacities.reset(1, HOURS_PER_YEAR, true);
    indirectCapacities.reset(1, HOURS_PER_YEAR, true);

    for (uint i = 0; i != HOURS_PER_YEAR; ++i)
    {
        directCapacities[0][i] = 1.;
        indirectCapacities[0][i] = 1.;
    }
    useLoopFlow = false;
    usePST = false;
    useHurdlesCost = false;
    transmissionCapacities = Data::tncEnabled;
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
    directCapacities.invalidate(true);
    indirectCapacities.invalidate(true);

    // invert NTC values
    directCapacities.swap(indirectCapacities);

    directCapacities.markAsModified();
    indirectCapacities.markAsModified();

    directCapacities.flush();
    indirectCapacities.flush();
}

bool AreaLink::isVisibleOnLayer(const size_t& layerID) const
{
    if (from && with)
        return from->isVisibleOnLayer(layerID) && with->isVisibleOnLayer(layerID);
    return false;
}

AreaLink* AreaAddLinkBetweenAreas(Area* area, Area* with, bool warning)
{
    /* Asserts */
    assert(area);
    assert(with);

    if (warning && area->id > with->id)
        logs.warning() << "Link: `" << area->id << "` - `" << with->id << "`: Invalid orientation";

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
static bool AreaLinksInternalLoadFromProperty(Study& study,
                                              AreaLink& link,
                                              const String& key,
                                              const String& value)
{
    if (key == "hurdles-cost")
        return value.to<bool>(link.useHurdlesCost);
    if (key == "loop-flow-fee") // backward compatibility with version 6.5.1
        return value.to<bool>(link.useLoopFlow);
    if (key == "loop-flow")
        return value.to<bool>(link.useLoopFlow);
    if (key == "use-phase-shifter")
        return value.to<bool>(link.usePST);
    if (key == "copper-plate")
    {
        bool copperPlate;
        if (value.to<bool>(copperPlate))
        {
            link.transmissionCapacities = (copperPlate) ? Data::tncInfinite : Data::tncEnabled;
            return true;
        }
        return false;
    }
    if (key == "asset-type")
    {
        if (value == "ac")
            link.assetType = atAC;
        else if (value == "dc")
            link.assetType = atDC;
        else if (value == "gaz")
            link.assetType = atGas;
        else if (value == "virt")
            link.assetType = atVirt;
        else if (value == "other")
            link.assetType = atOther;
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
            link.style = stPlain;
        else if (value == "dot")
            link.style = stDot;
        else if (value == "dash")
            link.style = stDash;
        else if (value == "dotdash")
            link.style = stDotDash;
        else
        {
            link.style = stPlain;
            return false;
        }
        return true;
    }
    if (key == "link-width")
    {
        link.linkWidth = Math::MinMax<int>(value.to<int>(), 1, 6);
        return true;
    }
    if (key == "colorr")
    {
        link.color[0] = Math::MinMax<int>(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "colorg")
    {
        link.color[1] = Math::MinMax<int>(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "colorb")
    {
        link.color[2] = Math::MinMax<int>(value.to<int>(), 0, 255);
        return true;
    }
    if (key == "transmission-capacities")
    {
        if (value == "enabled")
            link.transmissionCapacities = tncEnabled;
        else if (value == "infinite")
            link.transmissionCapacities = tncInfinite;
        else if (value == "ignore")
            link.transmissionCapacities = tncIgnore;
        else
        {
            link.transmissionCapacities = tncIgnore;
            return false;
        }
        return true;
    }

    if (key == "display-comments")
        return value.to<bool>(link.displayComments);
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
    if (study.header.version < 330)
    {
        // In releases prior 3.3, some useless fields were saved/loaded
        // (only required by the expansion mode)
        return (key == "thresholdmin") || (key == "thresholdmax") || (key == "investment");
    }
    return false;
}

} // anonymous namespace

void logLinkDataCheckError(Study& study, const AreaLink& link)
{
    logs.error() << "Link (" << link.from->name << "/" << link.with->name << "): Invalid values";
    study.gotFatalError = true;
}

void logLinkDataCheckErrorDirectIndirect(Study& study,
                                         const AreaLink& link,
                                         uint direct,
                                         uint indirect)
{
    logs.error() << "Link (" << link.from->name << "/" << link.with->name << "): Found " << direct
                 << " direct TS "
                 << " and " << indirect << " indirect TS, expected the same number";
    study.gotFatalError = true;
}

bool AreaLinksLoadFromFolder(Study& study, AreaList* l, Area* area, const AnyString& folder)
{
    // Assert
    assert(area);

    /* Initialize */
    String buffer;
    if (study.header.version < 320)
        buffer << folder << SEP << "ntc.ini";
    else
        buffer << folder << SEP << "properties.ini";

    IniFile ini;
    if (!ini.open(buffer))
        return 0;

    bool ret = true;
    String key;
    String value;

    // Browse all sections
    for (auto* s = ini.firstSection; s; s = s->next)
    {
        // Getting the name of the area
        buffer.clear();
        TransformNameIntoID(s->name, buffer);

        // Trying to find it
        Area* linkedWith = AreaListLFind(l, buffer.c_str());
        if (!linkedWith)
        {
            logs.error() << '`' << s->name << "`: Impossible to find the area";
            continue;
        }
        AreaLink* lnk = AreaAddLinkBetweenAreas(area, linkedWith);
        if (!lnk)
        {
            logs.error() << "Impossible to create a link between two areas";
            continue;
        }

        auto& link = *lnk;

        link.comments.clear();
        link.displayComments = true;

        ret = link.loadTimeSeries(study, folder) && ret;

        // Checks on loaded link's data
        if (study.usedByTheSolver)
        {
            // Short names for link's properties
            const uint nbDirectTS = link.directCapacities.width;
            const uint nbIndirectTS = link.indirectCapacities.width;
            if (nbDirectTS != nbIndirectTS)
            {
                logLinkDataCheckErrorDirectIndirect(study, link, nbDirectTS, nbIndirectTS);
                return false;
            }

            auto& directHurdlesCost = link.parameters[fhlHurdlesCostDirect];
            auto& indirectHurdlesCost = link.parameters[fhlHurdlesCostIndirect];
            auto& loopFlow = link.parameters[fhlLoopFlow];
            auto& PShiftMinus = link.parameters[fhlPShiftMinus];
            auto& PShiftPlus = link.parameters[fhlPShiftPlus];

            for (uint indexTS = 0; indexTS < nbDirectTS; ++indexTS)
            {
                auto& directCapacities = link.directCapacities[indexTS];
                auto& indirectCapacities = link.indirectCapacities[indexTS];

                // Checks on direct capacities
                for (int h = 0; h < HOURS_PER_YEAR; h++)
                {
                    if (directCapacities[h] < 0. || directCapacities[h] < loopFlow[h])
                    {
                        logLinkDataCheckError(study, link);
                        return false;
                    }
                }

                // Checks on indirect capacities
                for (int h = 0; h < HOURS_PER_YEAR; h++)
                {
                    if (indirectCapacities[h] < 0. || indirectCapacities[h] + loopFlow[h] < 0)
                    {
                        logLinkDataCheckError(study, link);
                        return false;
                    }
                }
            }
            // Checks on hurdle costs
            for (int h = 0; h < HOURS_PER_YEAR; h++)
            {
                if (directHurdlesCost[h] + indirectHurdlesCost[h] < 0)
                {
                    logLinkDataCheckError(study, link);
                    return false;
                }
            }

            // Checks on P. shift min and max
            for (int h = 0; h < HOURS_PER_YEAR; h++)
            {
                if (PShiftPlus[h] < PShiftMinus[h])
                {
                    logLinkDataCheckError(study, link);
                    return false;
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
            if (!AreaLinksInternalLoadFromProperty(study, link, key, value))
                logs.warning() << '`' << p->key << "`: Unknown property";
        }

        if (study.header.version < 400)
        {
            // Before 3.9, it was possible to set hurdle costs below
            // LINK_MINIMAL_HURDLE_COSTS_NOT_NULL which is a mistake (see virtual costs).
            // Starting from 3.9, the UI does not longer allow values below
            // LINK_MINIMAL_HURDLE_COSTS_NOT_NULL but we have to normalize the hurdle costs for
            // older studies. We can not directly use 0 it will bring too much damage to the results
            link.parameters.invalidate(true);
            auto& hurdleCostsD = link.parameters[Data::fhlHurdlesCostDirect];
            auto& hurdleCostsI = link.parameters[Data::fhlHurdlesCostIndirect];
            bool rounding = false;

            for (uint h = 0; h != link.parameters.height; ++h)
            {
                if (Math::Abs(hurdleCostsD[h]) < LINK_MINIMAL_HURDLE_COSTS_NOT_NULL)
                {
                    if (Math::Abs(hurdleCostsD[h]) < 1e-22)
                        hurdleCostsD[h] = 0.;
                    else
                    {
                        hurdleCostsD[h] = (hurdleCostsD[h] > 0.)
                                            ? +LINK_MINIMAL_HURDLE_COSTS_NOT_NULL
                                            : -LINK_MINIMAL_HURDLE_COSTS_NOT_NULL;
                        rounding = true;
                    }
                }
                if (Math::Abs(hurdleCostsI[h]) < LINK_MINIMAL_HURDLE_COSTS_NOT_NULL)
                {
                    if (Math::Abs(hurdleCostsI[h]) < 1e-22)
                        hurdleCostsI[h] = 0.;
                    else
                    {
                        hurdleCostsI[h] = (hurdleCostsI[h] > 0)
                                            ? +LINK_MINIMAL_HURDLE_COSTS_NOT_NULL
                                            : -LINK_MINIMAL_HURDLE_COSTS_NOT_NULL;
                        rounding = true;
                    }
                }
            }
            link.parameters.markAsModified();

            if (rounding)
            {
                CString<16, false> roundValue(LINK_MINIMAL_HURDLE_COSTS_NOT_NULL);
                roundValue.trimRight('0');
                if (roundValue.last() == '.')
                    roundValue.removeLast();
                logs.warning() << "link " << link.from->id << " / " << link.with->id
                               << ": hurdle costs < " << LINK_MINIMAL_HURDLE_COSTS_NOT_NULL
                               << " (but not null) have been rounding to " << roundValue;
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
            case Data::tncEnabled:
                break;
            case Data::tncIgnore:
            {
                // Ignore transmission capacities
                link.directCapacities.zero();
                link.indirectCapacities.zero();
                break;
            }
            case Data::tncInfinite:
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

        // memory swap
        link.flush();
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
        success = link.directCapacities.saveToCSVFile(filename, 6, false, true) && success;

        // Save indirect capacities time series

        filename.clear() << capacitiesFolder << SEP << link.with->id << "_indirect.txt";
        success = link.indirectCapacities.saveToCSVFile(filename, 6, false, true) && success;
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
            section->add("comments", link.comments);
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
        return false;

    if (!saveAreaLinksTimeSeriesToFolder(area, folder))
        return false;

    return true;
}

void AreaLinkRemove(AreaLink* link)
{
    if (!link)
        return;

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

void AreaLink::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += sizeof(AreaLink);
    Matrix<>::EstimateMemoryUsage(u, fhlMax, HOURS_PER_YEAR);

    // From the solver
    u.requiredMemoryForInput += 1 * 1024 * 1024;
}

Yuni::uint64 AreaLink::memoryUsage() const
{
    Yuni::uint64 to_return = sizeof(AreaLink);
    to_return += parameters.valuesMemoryUsage();
    to_return += directCapacities.valuesMemoryUsage();
    to_return += indirectCapacities.valuesMemoryUsage();

    return to_return;
}

bool AreaLink::invalidate(bool reload) const
{
    return parameters.invalidate(reload) && directCapacities.invalidate(reload)
           && indirectCapacities.invalidate(reload);
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

void AreaLink::flush()
{
    parameters.flush();
    directCapacities.flush();
    indirectCapacities.flush();
    timeseriesNumbers.flush();
}

} // namespace Data
} // namespace Antares
