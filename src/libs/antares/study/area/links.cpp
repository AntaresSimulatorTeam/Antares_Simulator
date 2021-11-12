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
    //
    from->links[with->id] = this;

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

void logLinkDataCheckError(Study& study, AreaLink& link)
{
    logs.error() << "Link (" << link.from->name << "/" << link.with->name
        << "): Invalid values";
    study.gotFatalError = true;
}

bool linkLoadTimeSeries_version_under_320(AreaLink& link, const AnyString& folder, Study& study)
{
    String buffer;
    bool ret = true;

    // gp : refactor this part : avoid code duplication
    link.parameters.resize(fhlMax, HOURS_PER_YEAR);
    link.parameters.zero();
    if (link.parameters.jit)
    {
        link.parameters.jit->options |= Matrix<>::optFixedSize;
        link.parameters.markAsModified();
    }

    link.directCapacities.resize(1, HOURS_PER_YEAR);
    link.directCapacities.zero();
    if (link.directCapacities.jit)
    {
        link.directCapacities.jit->options |= Matrix<>::optFixedSize;
        link.directCapacities.markAsModified();
    }

    link.indirectCapacities.resize(1, HOURS_PER_YEAR);
    link.indirectCapacities.zero();
    if (link.indirectCapacities.jit)
    {
        link.indirectCapacities.jit->options |= Matrix<>::optFixedSize;
        link.indirectCapacities.markAsModified();
    }

    uint loadOptions = Matrix<>::optFixedSize | Matrix<>::optImmediate;
    // NTC direct
    buffer.clear() << folder << SEP << link.with->id << SEP << "direct."
        << study.inputExtension;
    ret = link.directCapacities.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    link.directCapacities.markAsModified();

    // NTC indirect
    buffer.clear() << folder << SEP << link.with->id << SEP << "indirect."
        << study.inputExtension;
    ret = link.indirectCapacities.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    link.indirectCapacities.markAsModified();

    // Impedances
    buffer.clear() << folder << SEP << link.with->id << SEP << "impedances."
        << study.inputExtension;
    Matrix<double> tmp;
    ret = tmp.loadFromCSVFile(buffer, 1, HOURS_PER_YEAR, loadOptions) && ret;
    link.parameters.pasteToColumn(fhlImpedances, tmp[0]);
    link.parameters.markAsModified();

    return ret;
}

bool linkLoadTimeSeries_version_320_to_630(AreaLink& link, const AnyString& folder)
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
    link.parameters.resize(fhlMax, HOURS_PER_YEAR);
    link.directCapacities.resize(1, HOURS_PER_YEAR);
    link.indirectCapacities.resize(1, HOURS_PER_YEAR);

    // Initialize link's data container
    link.parameters.zero();
    link.directCapacities.zero();
    link.indirectCapacities.zero();

    // Load link's data
    buffer.clear() << folder << SEP << link.with->id << ".txt";
    Matrix<> tmpMatrix;
    ret = tmpMatrix.loadFromCSVFile(buffer, 5, HOURS_PER_YEAR, Matrix<>::optFixedSize | Matrix<>::optImmediate) && ret;

    // Store data into link's data container
    for (int h = 0; h < HOURS_PER_YEAR; h++)
    {
        link.directCapacities[0][h] = tmpMatrix[0][h];
        link.indirectCapacities[0][h] = tmpMatrix[1][h];
        link.parameters[fhlImpedances][h] = tmpMatrix[2][h];
        link.parameters[fhlHurdlesCostDirect][h] = tmpMatrix[3][h];
        link.parameters[fhlHurdlesCostIndirect][h] = tmpMatrix[4][h];
    }

    if (enabledModeIsChanged)
        JIT::enabled = true; // Back to the previous loading mode.

    return ret;
}

/*
            buffer.clear() << folder << SEP << link.with->id << ".txt";
            link.setPathToDataFile(buffer);
            ret = link.loadDataFromCSVfile(Matrix<>::optFixedSize | Matrix<>::optImmediate) && ret;
*/
bool linkLoadTimeSeries_version_630_to_820(AreaLink& link, const AnyString& folder)
{
    String buffer;
    bool ret = true;
    
    buffer.clear() << folder << SEP << link.with->id << ".txt";
    link.setPathToDataFile(buffer);
    ret = link.loadDataFromCSVfile(Matrix<>::optFixedSize | Matrix<>::optImmediate) && ret;

    return ret;
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
            ret = linkLoadTimeSeries_version_under_320(link, folder, study) && ret;
        else if (study.header.version < 630)
            ret = linkLoadTimeSeries_version_320_to_630(link, folder) && ret;
        else
            ret = linkLoadTimeSeries_version_630_to_820(link, folder) && ret;

        // Checks on loaded link's data
        if (study.usedByTheSolver)
        {
            // Short names for link's properties
            auto& directCapacities = link.directCapacities[0];
            auto& indirectCapacities = link.indirectCapacities[0];
            auto& directHurdlesCost = link.parameters[fhlHurdlesCostDirect];
            auto& indirectHurdlesCost = link.parameters[fhlHurdlesCostIndirect];
            auto& loopFlow = link.parameters[fhlLoopFlow];
            auto& PShiftMinus = link.parameters[fhlPShiftMinus];
            auto& PShiftPlus = link.parameters[fhlPShiftPlus];
            
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
            if (not link.useHurdlesCost || not study.parameters.include.hurdleCosts)
            {
                link.parameters.columnToZero(Data::fhlHurdlesCostDirect);
                link.parameters.columnToZero(Data::fhlHurdlesCostIndirect);
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
                link.directCapacities.zero();
                link.indirectCapacities.zero();
                break;
            }
            case Data::tncInfinite:
            {
                // Copper plate mode
                auto infinity = +std::numeric_limits<double>::infinity();
                link.directCapacities.fillColumn(0, +infinity);
                link.indirectCapacities.fillColumn(0, +infinity);
                break;
            }
            }
        }

        // memory swap
        link.flush();
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
    IniFile ini;

    bool ret = true;

    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);

        auto* section = ini.addSection(link.with->id);

        section->add("hurdles-cost", link.useHurdlesCost);
        section->add("loop-flow", link.useLoopFlow);
        section->add("use-phase-shifter", link.usePST);
        section->add("transmission-capacities", transmissionCapacitiesToString(link.transmissionCapacities));
        section->add("asset-type", assetTypeToString(link.assetType));
        section->add("link-style", styleToString(link.style));
        section->add("link-width", link.linkWidth);
        section->add("colorr", link.color[0]);
        section->add("colorg", link.color[1]);
        section->add("colorb", link.color[2]);
        section->add("display-comments", link.displayComments);
        if (not link.comments.empty())
            section->add("comments", link.comments);
        section->add("filter-synthesis", datePrecisionIntoString(link.filterSynthesis));
        section->add("filter-year-by-year", datePrecisionIntoString(link.filterYearByYear));
        
        // NTC
        filename.clear() << folder << SEP << link.with->id << ".txt";

        Matrix<> tmpMatrixToPrint;

        // Matrix to print's number of columns
        uint nbColumnsOfMatrixToPrint = link.parameters.width + link.directCapacities.width + link.indirectCapacities.width;

        // Matrix to print's number of lines
        assert(link.parameters.height == link.directCapacities.height);
        assert(link.parameters.height == link.indirectCapacities.height);
        uint nbLinesOfMatrixToPrint = link.parameters.height;
        
        tmpMatrixToPrint.resize(nbColumnsOfMatrixToPrint, nbLinesOfMatrixToPrint);
        tmpMatrixToPrint.zero();

        // Fill the matrix to print
        for (int h = 0; h < nbLinesOfMatrixToPrint; h++)
        {
            tmpMatrixToPrint[0][h] = link.directCapacities[0][h];
            tmpMatrixToPrint[1][h] = link.indirectCapacities[0][h];
            tmpMatrixToPrint[2][h] = link.parameters[fhlHurdlesCostDirect][h];
            tmpMatrixToPrint[3][h] = link.parameters[fhlHurdlesCostIndirect][h];
            tmpMatrixToPrint[4][h] = link.parameters[fhlImpedances][h];
            tmpMatrixToPrint[5][h] = link.parameters[fhlLoopFlow][h];
            tmpMatrixToPrint[6][h] = link.parameters[fhlPShiftMinus][h];
            tmpMatrixToPrint[7][h] = link.parameters[fhlPShiftPlus][h];
        }
        ret = tmpMatrixToPrint.saveToCSVFile(filename);
    }

    filename.clear() << folder << SEP << "properties.ini";
    if (! ini.save(filename))
        return false;

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
    Yuni::uint64 to_return = sizeof(AreaLink);
    to_return += parameters.valuesMemoryUsage();
    to_return += directCapacities.valuesMemoryUsage();
    to_return += indirectCapacities.valuesMemoryUsage();

    return to_return;
}

bool AreaLink::invalidate(bool reload) const
{
    return parameters.invalidate(reload) && directCapacities.invalidate(reload)
                                         && indirectCapacities.invalidate(reload) ;
}

void AreaLink::markAsModified() const
{
    parameters.markAsModified();
    directCapacities.markAsModified();
    indirectCapacities.markAsModified();
}

bool AreaLink::loadDataFromCSVfile(uint loadOptions)
{
    // Load link's data
    Matrix<> tmpMatrix;
    if (not tmpMatrix.loadFromCSVFile(pathToDataFile, 8, HOURS_PER_YEAR, loadOptions))
        return false;

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

String AreaLink::getName() const
{
    String s;
    s << from->name << "/" << with->name;
    return s;
}

void AreaLink::flush()
{
    parameters.flush();
    directCapacities.flush();
    indirectCapacities.flush();
}

} // namespace Data
} // namespace Antares
