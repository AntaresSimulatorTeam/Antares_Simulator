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

using namespace Yuni;
using namespace Antares;

#define SEP (IO::Separator)

namespace Antares
{
namespace Data
{
AreaLink::AreaLink() :
 from(nullptr),
 with(nullptr),
 data(fhlMax, HOURS_PER_YEAR),
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
    data.reset(fhlMax, HOURS_PER_YEAR, true);

    for (uint i = 0; i != data.height; ++i)
    {
        data[fhlNTCDirect][i] = 1.;
        data[fhlNTCIndirect][i] = 1.;
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
    //
    from->links[with->id] = this;

    // Making sure that we have the data
    data.invalidate(true);

    // inverting NTC values
    double* tmp = new double[data.height];
    size_t siz = sizeof(double) * data.height;

    memcpy(tmp, data[fhlNTCDirect], siz);
    memcpy(data[fhlNTCDirect], data[fhlNTCIndirect], siz);
    memcpy(data[fhlNTCIndirect], tmp, siz);
    delete tmp;

    data.markAsModified();
    data.flush();
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

    AreaLink* link = new AreaLink();
    link->from = area;
    link->with = with;
    area->links[with->id] = link;
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
        link.filterSynthesis = StringToFilter(value);
        return true;
    }
    if (key == "filter-year-by-year")
    {
        link.filterYearByYear = StringToFilter(value);
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
    if (not ini.open(buffer))
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

        if (study.header.version < 320)
        {
            link.data.resize(fhlMax, HOURS_PER_YEAR);
            link.data.zero();
            if (link.data.jit)
            {
                link.data.jit->options |= Matrix<>::optFixedSize;
                link.data.markAsModified();
            }

            Matrix<double> tmp;
            // NTC
            buffer.clear() << folder << SEP << link.with->id << SEP << "direct."
                           << study.inputExtension;
            ret = tmp.loadFromCSVFile(
                    buffer, 1, 8760, Matrix<>::optFixedSize | Matrix<>::optImmediate)
                  && ret;
            link.data.pasteToColumn(fhlNTCDirect, tmp[0]);

            buffer.clear() << folder << SEP << link.with->id << SEP << "indirect."
                           << study.inputExtension;
            ret = tmp.loadFromCSVFile(
                    buffer, 1, 8760, Matrix<>::optFixedSize | Matrix<>::optImmediate)
                  && ret;
            link.data.pasteToColumn(fhlNTCIndirect, tmp[0]);

            buffer.clear() << folder << SEP << link.with->id << SEP << "impedances."
                           << study.inputExtension;
            ret = tmp.loadFromCSVFile(
                    buffer, 1, 8760, Matrix<>::optFixedSize | Matrix<>::optImmediate)
                  && ret;
            link.data.pasteToColumn(fhlImpedances, tmp[0]);

            link.data.markAsModified();
        }
        else if (study.header.version < 630)
        {
            bool enabledModeIsChanged = false;
            if (JIT::enabled)
            {
                JIT::enabled = false; // Allowing to read the area's daily max power
                enabledModeIsChanged = true;
            }
            buffer.clear() << folder << SEP << link.with->id << ".txt";
            ret
              = link.data.loadFromCSVFile(buffer, 5, HOURS_PER_YEAR, Matrix<>::optFixedSize) && ret;
            double temp[5][HOURS_PER_YEAR];

            auto& TCD = link.data[fhlNTCDirect];
            auto& TCI = link.data[fhlNTCIndirect];
            auto& impedances = link.data[2];
            auto& HCD = link.data[3];
            auto& HCI = link.data[4];
            for (int i = 0; i < HOURS_PER_YEAR; i++)
            {
                temp[fhlNTCDirect][i] = TCD[i];
                temp[fhlNTCIndirect][i] = TCI[i];
                temp[fhlImpedances][i] = impedances[i];
                temp[fhlHurdlesCostDirect][i] = HCD[i];
                temp[fhlHurdlesCostIndirect][i] = HCI[i];
            }
            link.data.resize(fhlMax, HOURS_PER_YEAR);
            link.data.zero();
            for (int i = 0; i < HOURS_PER_YEAR; i++)
            {
                link.data[fhlNTCDirect][i] = temp[fhlNTCDirect][i];
                link.data[fhlNTCIndirect][i] = temp[fhlNTCIndirect][i];
                link.data[fhlImpedances][i] = temp[fhlImpedances][i];
                link.data[fhlHurdlesCostDirect][i] = temp[fhlHurdlesCostDirect][i];
                link.data[fhlHurdlesCostIndirect][i] = temp[fhlHurdlesCostIndirect][i];
            }
            if (enabledModeIsChanged)
                JIT::enabled = true; // Back to the previous loading mode.
        }
        else
        {
            buffer.clear() << folder << SEP << link.with->id << ".txt";
            ret = link.data.loadFromCSVFile(buffer, fhlMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
                  && ret;
        }
        if (study.usedByTheSolver)
        {
            bool fatal = false;
            for (int colNum = 0; colNum < fhlMax; colNum++)
            {
                switch (colNum)
                {
                case Data::fhlNTCDirect:
                {
                    auto& col = link.data[colNum];
                    auto& colLoopFlow = link.data[Data::fhlLoopFlow];
                    for (int i = 0; i < HOURS_PER_YEAR; i++)
                    {
                        if ((col[i] < 0.) || (col[i] < colLoopFlow[i]))
                        {
                            fatal = true;
                            break;
                        }
                    }
                    break;
                }
                case Data::fhlNTCIndirect:
                {
                    auto& col = link.data[colNum];
                    auto& colLoopFlow = link.data[Data::fhlLoopFlow];
                    for (int i = 0; i < HOURS_PER_YEAR; i++)
                    {
                        if ((col[i] < 0.) || ((col[i] + colLoopFlow[i]) < 0))
                        {
                            fatal = true;
                            break;
                        }
                    }
                    break;
                }
                case Data::fhlHurdlesCostDirect:
                {
                    auto& col = link.data[colNum];
                    auto& colHurdleCostsIndirect = link.data[Data::fhlHurdlesCostIndirect];
                    for (int i = 0; i < HOURS_PER_YEAR; i++)
                    {
                        if (col[i] + colHurdleCostsIndirect[i] < 0)
                        {
                            fatal = true;
                            break;
                        }
                    }
                    break;
                }
                case Data::fhlHurdlesCostIndirect:
                {
                    auto& col = link.data[colNum];
                    auto& colHurdleCostsDirect = link.data[Data::fhlHurdlesCostDirect];
                    for (int i = 0; i < HOURS_PER_YEAR; i++)
                    {
                        if (col[i] + colHurdleCostsDirect[i] < 0)
                        {
                            fatal = true;
                            break;
                        }
                    }
                    break;
                }
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case Data::fhlPShiftPlus:
                {
                    auto& col = link.data[colNum];
                    auto& colPShiftMinus = link.data[Data::fhlPShiftMinus];
                    for (int i = 0; i < HOURS_PER_YEAR; i++)
                    {
                        if (col[i] < colPShiftMinus[i])
                        {
                            fatal = true;
                            break;
                        }
                    }
                    break;
                }
                }
            }
            if (fatal)
            {
                logs.error() << "Link (" << link.from->name << "/" << link.with->name
                             << "): Invalid values";
                study.gotFatalError = true;
                return false;
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
            // LINK_MINIMAL_HURDLE_COSTS_NOT_NULL which is a mistake (see virtual costs) Starting
            // from 3.9, the UI does not longer allow values below
            // LINK_MINIMAL_HURDLE_COSTS_NOT_NULL but we have to normalize the hurdle costs for
            // older studies. We can not directly use 0 it will bring too much damage to the results
            link.data.invalidate(true);
            auto& hurdleCostsD = link.data[Data::fhlHurdlesCostDirect];
            auto& hurdleCostsI = link.data[Data::fhlHurdlesCostIndirect];
            bool rounding = false;

            for (uint h = 0; h != link.data.height; ++h)
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
            link.data.markAsModified();

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
            if (not link.useHurdlesCost || not study.parameters.include.hurdleCosts)
            {
                link.data.columnToZero(Data::fhlHurdlesCostDirect);
                link.data.columnToZero(Data::fhlHurdlesCostIndirect);
            }
            // Global Optimization override
            if (study.parameters.transmissionCapacities != Data::tncEnabled)
                link.transmissionCapacities = study.parameters.transmissionCapacities;

            switch (link.transmissionCapacities)
            {
            case Data::tncEnabled:
                break;
            case Data::tncIgnore:
            {
                // Ignore transmission capacities
                link.data.columnToZero(Data::fhlNTCDirect);
                link.data.columnToZero(Data::fhlNTCIndirect);
                break;
            }
            case Data::tncInfinite:
            {
                // Copper plate mode
                auto infinity = +std::numeric_limits<double>::infinity();
                link.data.fillColumn(Data::fhlNTCDirect, +infinity);
                link.data.fillColumn(Data::fhlNTCIndirect, +infinity);
                break;
            }
            }
        }

        // memory swap
        link.data.flush();
    }

    return ret;
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

    String filename;
    Clob buffer;

    bool ret = true;

    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);

        buffer << '[' << link.with->id << "]\n";
        // Properties
        buffer << "hurdles-cost = " << (link.useHurdlesCost ? "true\n" : "false\n");
        buffer << "loop-flow = " << (link.useLoopFlow ? "true\n" : "false\n");
        buffer << "use-phase-shifter = " << (link.usePST ? "true\n" : "false\n");
        switch (link.transmissionCapacities)
        {
        case Data::tncEnabled:
            buffer << "transmission-capacities = enabled\n";
            break;
        case Data::tncIgnore:
            buffer << "transmission-capacities = ignore\n";
            break;
        case Data::tncInfinite:
            buffer << "transmission-capacities = infinite\n";
            break;
        }

        switch (link.assetType)
        {
        case Data::atAC:
            buffer << "asset-type = ac\n";
            break;
        case Data::atDC:
            buffer << "asset-type = dc\n";
            break;
        case Data::atGas:
            buffer << "asset-type = gaz\n";
            break;
        case Data::atVirt:
            buffer << "asset-type = virt\n";
            break;
        case Data::atOther:
            buffer << "asset-type = other\n";
            break;
        }
        switch (link.style)
        {
        case Data::stPlain:
            buffer << "link-style = plain\n";
            break;
        case Data::stDot:
            buffer << "link-style = dot\n";
            break;
        case Data::stDash:
            buffer << "link-style = dash\n";
            break;
        case Data::stDotDash:
            buffer << "link-style = dotdash\n";
            break;
        }
        buffer << "link-width = " << link.linkWidth << "\n";
        buffer << "colorr = " << link.color[0] << "\n";
        buffer << "colorg = " << link.color[1] << "\n";
        buffer << "colorb = " << link.color[2] << "\n";

        buffer << "display-comments = " << (link.displayComments ? "true\n" : "false\n");
        if (not link.comments.empty())
            buffer << "comments = " << link.comments << '\n';
        buffer << "filter-synthesis = ";
        AppendFilterToString(buffer, link.filterSynthesis);
        buffer << '\n';
        buffer << "filter-year-by-year = ";
        AppendFilterToString(buffer, link.filterYearByYear);
        buffer << '\n';
        buffer << '\n';

        // NTC
        filename.clear() << folder << SEP << link.with->id << ".txt";
        ret = link.data.saveToCSVFile(filename);
    }

    filename.clear() << folder << SEP << "properties.ini";
    IO::File::Stream file;
    if (!file.openRW(filename))
    {
        logs.error() << "I/O error: impossible to write " << filename;
        return false;
    }
    file << buffer;

    return ret;
}

void AreaLinkRemove(AreaLink* l)
{
    if (!l)
        return;

    // Asserts
    assert(l->from);
    assert(l->with);

    Area* f = l->from;
    if (f && !f->links.empty())
    {
        f->detachLinkFromID(l->with->id);
        f->detachLinkFromID(l->from->id);
    }

    Area* t = l->with;
    if (t && !t->links.empty())
    {
        t->detachLinkFromID(l->with->id);
        t->detachLinkFromID(l->from->id);
    }

    delete l;
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
    return (sizeof(AreaLink) + data.valuesMemoryUsage());
}

bool AreaLink::invalidate(bool reload) const
{
    return data.invalidate(reload);
}

void AreaLink::markAsModified() const
{
    data.markAsModified();
}

String AreaLink::getName() const
{
    String s;
    s << from->name << "/" << with->name;
    return s;
}

} // namespace Data
} // namespace Antares
