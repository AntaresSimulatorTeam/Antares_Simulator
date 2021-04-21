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

#include "../../antares.h"
#include <yuni/io/file.h>
#include <yuni/core/string.h>
#include "../study.h"
#include <assert.h>
#include "area.h"
#include "../../array/array1d.h"
#include "../../inifile/inifile.h"
#include "../../logs.h"
#include "../memory-usage.h"
#include "../../config.h"
#include "../filter.h"
#include "constants.h"

#define SEP IO::Separator

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace // anonymous
{
static bool AreaListLoadThermalDataFromFile(AreaList& list, const Clob& filename)
{
    // Reset to 0
    list.each([](Data::Area& area) {
        area.thermal.unsuppliedEnergyCost = 0.;
        area.thermal.spilledEnergyCost = 0.;
    });

    IniFile ini;
    // Try to load the file
    if (not ini.open(filename))
        return false;

    auto* section = ini.find("unserverdenergycost");

    // Try to find the section
    if (section and section->firstProperty)
    {
        AreaName id;
        for (IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            id.clear();
            TransformNameIntoID(p->key, id);
            Area* area = list.find(id);
            if (area)
            {
                // MBO 15/04/2014
                // limit unsuppliedEnergyCost to 5.e4
                area->thermal.unsuppliedEnergyCost = p->value.to<double>();
                // if (area->thermal.unsuppliedEnergyCost < 0.)
                // logs.error() << "Thermal: `" << area->name << "`: The unsupplied energy cost must
                // be positive";

                // New scheme
                // MBO 30/10/2014 - Allow negative values
                if (Math::Abs(area->thermal.unsuppliedEnergyCost) < 5.e-3)
                    area->thermal.unsuppliedEnergyCost = 0.;
                else
                {
                    if (area->thermal.unsuppliedEnergyCost > 5.e4)
                        area->thermal.unsuppliedEnergyCost = 5.e4;
                    else
                    {
                        if (area->thermal.unsuppliedEnergyCost < -5.e4)
                            area->thermal.unsuppliedEnergyCost = -5.e4;
                    }
                }
            }
            else
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
        }
    }

    section = ini.find("spilledenergycost");

    // Try to find the section
    if (section and section->firstProperty)
    {
        AreaName id;
        for (IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            id.clear();
            TransformNameIntoID(p->key, id);
            auto* area = list.find(id);
            if (area)
            {
                // MBO 15/04/2014
                // limit spilledEnergyCost to 5.e4
                area->thermal.spilledEnergyCost = p->value.to<double>();
                // if (area->thermal.spilledEnergyCost < 0.)
                // logs.error() << "Thermal: `" << area->name << "`: The spilled energy cost must be
                // positive";

                // MBO 30/10/2014 - allow negative values
                if (Math::Abs(area->thermal.spilledEnergyCost) < 5.e-3)
                    area->thermal.spilledEnergyCost = 0.;
                else
                {
                    if (area->thermal.spilledEnergyCost > 5.e4)
                        area->thermal.spilledEnergyCost = 5.e4;
                    else
                    {
                        if (area->thermal.spilledEnergyCost < -5.e4)
                            area->thermal.spilledEnergyCost = -5.e4;
                    }
                }
            }
            else
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
        }
    }

    // This is not a warning !
    // We may have an empty INI file with 0 everywhere
    // logs.warning() << '`' << filename << "`: The section `unsuppliedenergycost` can not be
    // found";
    return true;
}

static bool AreaListSaveThermalDataToFile(const AreaList& list, const AnyString& filename)
{
    Clob data;
    data << "[unserverdenergycost]\n";
    list.each([&](const Data::Area& area) {
        // 0 values are skipped
        if (not Math::Zero(area.thermal.unsuppliedEnergyCost))
            data << area.id << " = " << area.thermal.unsuppliedEnergyCost << '\n';
    });

    data << "\n[spilledenergycost]\n";
    list.each([&](const Data::Area& area) {
        // 0 values are skipped
        if (not Math::Zero(area.thermal.spilledEnergyCost))
            data << area.id << " = " << area.thermal.spilledEnergyCost << '\n';
    });

    IO::File::Stream file;
    if (file.openRW(filename))
    {
        file << data;
    }
    else
    {
        logs.error() << "I/O error: impossible to write " << filename;
        return false;
    }
    return true;
}

static bool AreaListSaveToFolderSingleArea(const Area& area, Clob& buffer, const AnyString& folder)
{
    bool ret = true;

    // A specific folder for general data
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id;
    if (!IO::Directory::Create(buffer))
    {
        logs.error() << "I/O Error: Impossible to create: " << buffer;
        ret = false;
    }

    // Data related to User Interface
    if (area.ui and area.ui->modified())
    {
        buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id << SEP
                       << "ui.ini";
        ret = area.ui->saveToFile(buffer) and ret;
    }

    // Nodal optimization
    {
        buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id << SEP
                       << "optimization.ini";
        IO::File::Stream file;
        if (file.openRW(buffer))
        {
            buffer.clear();
            buffer << "[nodal optimization]\n";
            buffer << "non-dispatchable-power = "
                   << ((bool)((area.nodalOptimization & anoNonDispatchPower) != 0) ? "true\n"
                                                                                   : "false\n");
            buffer << "dispatchable-hydro-power = "
                   << ((bool)((area.nodalOptimization & anoDispatchHydroPower) != 0) ? "true\n"
                                                                                     : "false\n");
            buffer << "other-dispatchable-power = "
                   << ((bool)((area.nodalOptimization & anoOtherDispatchPower) != 0) ? "true\n"
                                                                                     : "false\n");
            buffer << "spread-unsupplied-energy-cost = " << area.spreadUnsuppliedEnergyCost << '\n';
            buffer << "spread-spilled-energy-cost = " << area.spreadSpilledEnergyCost << '\n';

            buffer << '\n';
            buffer << "[filtering]\n";
            buffer << "filter-synthesis = ";
            AppendFilterToString(buffer, area.filterSynthesis);
            buffer << '\n';
            buffer << "filter-year-by-year = ";
            AppendFilterToString(buffer, area.filterYearByYear);
            buffer << '\n';
            buffer << '\n';
            file << buffer;
        }
        else
        {
            logs.error() << "I/O error: impossible to write " << buffer;
            ret = false;
        }
    }

    // Reserves: primary, strategic, dsm, d-1...
    buffer.clear() << folder << SEP << "input" << SEP << "reserves" << SEP << area.id << ".txt";
    ret = area.reserves.saveToCSVFile(buffer) and ret;

    // Fatal hors hydro
    buffer.clear() << folder << SEP << "input" << SEP << "misc-gen" << SEP << "miscgen-" << area.id
                   << ".txt";
    ret = area.miscGen.saveToCSVFile(buffer) and ret;

    // NTC
    buffer.clear() << folder << SEP << "input" << SEP << "links" << SEP << area.id;
    ret = AreaLinksSaveToFolder(&area, buffer.c_str()) and ret;

    // Load
    {
        if (area.load.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.load.prepro->saveToFolder(buffer) and ret;
        }
        if (area.load.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "series";
            ret = DataSeriesLoadSaveToFolder(area.load.series, area.id, buffer.c_str()) and ret;
        }
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.solar.prepro->saveToFolder(buffer) and ret;
        }
        if (area.solar.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "series";
            ret = DataSeriesSolarSaveToFolder(area.solar.series, area.id, buffer.c_str()) and ret;
        }
    }

    // Hydro
    {
        buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "allocation" << SEP
                       << area.id << ".ini";
        ret = area.hydro.allocation.saveToFile(buffer) and ret;

        if (area.hydro.prepro) // Hydro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "prepro";
            ret = area.hydro.prepro->saveToFolder(area.id, buffer.c_str()) and ret;
        }
        if (area.hydro.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "series";
            ret = area.hydro.series->saveToFolder(area.id, buffer) and ret;
        }
    }

    // Wind
    {
        if (area.wind.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.wind.prepro->saveToFolder(buffer) and ret;
        }
        if (area.wind.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "series";
            ret = DataSeriesWindSaveToFolder(area.wind.series, area.id, buffer.c_str()) and ret;
        }
    }

    // Thermal cluster list
    {
        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "clusters" << SEP
                       << area.id;
        ret = ThermalClusterListSaveToFolder(&area.thermal.list, buffer) and ret;

        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "prepro";
        ret = ThermalClusterListSavePreproToFolder(&area.thermal.list, buffer) and ret;
        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "series";
        ret = ThermalClusterListSaveDataSeriesToFolder(&area.thermal.list, buffer) and ret;
    }

    return ret;
}

} // anonymous namespace

AreaList::AreaList(Study& study) : byIndex(nullptr), pStudy(study)
{
}

AreaList::~AreaList()
{
    clear();
}

AreaLink* AreaListAddLink(AreaList* l, const char area[], const char with[], bool warning)
{
    // Asserts
    assert(l);
    assert(area);
    assert(with);

    if (!l->empty())
    {
        logs.debug() << "    . " << area << " -> " << with;

        AreaName name;
        AreaName givenName = area;
        TransformNameIntoID(givenName, name);
        Area* a = AreaListLFind(l, name.c_str());
        if (a)
        {
            givenName = with;
            name.clear();
            TransformNameIntoID(givenName, name);
            Area* b = l->find(name);
            if (b and !a->findExistingLinkWith(*b))
                return AreaAddLinkBetweenAreas(a, b, warning);
        }
    }
    return nullptr;
}

AreaLink* AreaList::findLink(const AreaName& area, const AreaName& with)
{
    auto i = areas.find(area);
    if (i != areas.end())
    {
        auto j = areas.find(with);
        if (j != areas.end())
            return (*(i->second)).findExistingLinkWith(*(j->second));
    }
    return nullptr;
}

const AreaLink* AreaList::findLink(const AreaName& area, const AreaName& with) const
{
    auto i = areas.find(area);
    if (i != areas.end())
    {
        auto j = areas.find(with);
        if (j != areas.end())
            return (*(i->second)).findExistingLinkWith(*(j->second));
    }
    return nullptr;
}

void AreaList::clear()
{
    delete[] byIndex;
    byIndex = nullptr;

    if (not areas.empty())
    {
        Area::Map copy;
        copy.swap(areas);

        auto end = copy.end();
        for (auto i = copy.begin(); i != end; ++i)
            delete i->second;
    }
}

void AreaList::rebuildIndexes()
{
    delete[] byIndex;

    if (areas.empty())
    {
        byIndex = nullptr;
    }
    else
    {
        typedef Area* AreaWeakPtr;
        byIndex = new AreaWeakPtr[areas.size()];

        uint indx = 0;
        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i, ++indx)
        {
            Area* area = i->second;
            byIndex[indx] = area;
            area->index = indx;
        }
    }
}

Area* AreaList::add(Area* a)
{
    if (a)
    {
        // Indexing the area
        a->index = (uint)areas.size();

// Adding the area in the list
#ifndef NDEBUG
        uint count = (uint)areas.size();
#endif

        areas[a->id] = a;
        rebuildIndexes();

#ifndef NDEBUG
        assert(areas.size() == (count + 1) and "Invalid count of areas in the map");
#endif
    }
    return a;
}

Area* AreaListAddFromName(AreaList& list, const AnyString& name, uint nbParallelYears)
{
    // Initializing names
    AreaName cname;
    AreaName lname;
    cname = name;
    TransformNameIntoID(cname, lname);

    // Add the area
    return AreaListAddFromNames(list, cname, lname, nbParallelYears);
}

Area* AreaListAddFromNames(AreaList& list,
                           const AnyString& name,
                           const AnyString& lname,
                           uint nbParallelYears)
{
    if (!name or !lname)
        return nullptr;
    // Look up
    if (!AreaListLFind(&list, lname.c_str()))
    {
        // Creating the area
        Area* area = new Area(name, lname, nbParallelYears);
        // Adding it
        Area* ret = list.add(area);
        if (!ret)
        {
            // The area has not been added, the pointer is not referenced anywhere
            delete area;
            return nullptr;
        }
        return ret;
    }
    else
        logs.warning() << "The area `" << name << "` can not be added (dupplicate)";
    return nullptr;
}

bool AreaList::loadListFromFile(const AnyString& filename)
{
    IO::File::Stream file;
    if (!file.open(filename))
    {
        logs.error() << "I/O error: " << filename << ": Impossible to open the file";
        return false;
    }

    // Log entry
    logs.info() << "  Loading the area list from `" << filename << '`';

    // Initialization of the strings
    AreaName name;
    AreaName lname;
    // Each lines in the file
    String buffer;
    buffer.reserve(1024 /* to force the allocation */);
    uint line = 0;
    while (file.readline(buffer))
    {
        ++line;
        // The area name
        name = buffer;
        name.trim(" \t\n\r");
        if (name.empty())
            continue;

        if (pStudy.header.version < 390)
        {
            // before 390, all double-spaces from ids were suppressed
            String rename;
            TransformNameIntoNameBefore390(name, rename);
            name = rename;
            name.replace("  ", " ");
        }

        lname.clear();
        TransformNameIntoID(name, lname);
        if (lname.empty())
        {
            logs.warning() << "ignoring invalid area name: `" << name << "`, " << filename
                           << ": line " << line;
            continue;
        }
        // Add the area in the list
        AreaListAddFromNames(*this, name, lname, pStudy.maxNbYearsInParallel);
    }

    switch (areas.size())
    {
    case 0:
        logs.info() << "  No area found";
        break;
    case 1:
        logs.info() << "  1 area found";
        break;
    default:
        logs.info() << "  " << areas.size() << " areas found";
    }
    return true;
}

bool AreaList::saveLinkListToFile(const AnyString& filename) const
{
    IO::File::Stream file;
    if (not file.openRW(filename))
        return false;

    each([&](const Data::Area& area) {
        file << area.id << '\n';
        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
            file << '\t' << (i->second)->with->id << '\n';
    });
    return true;
}

bool AreaList::saveListToFile(const AnyString& filename) const
{
    if (!filename)
        return false;

    Clob data;
    {
        // Preparing a new list of areas, sorted by their name
        typedef std::list<std::string> List;
        List list;
        {
            auto end = areas.end();
            for (auto i = areas.begin(); i != end; ++i)
                list.push_back((i->second)->name.c_str());
            list.sort();
        }
        {
            auto end = list.end();
            for (auto i = list.begin(); i != end; ++i)
                data << *i << '\n';
        }
    }

    // Writing data into the appropriate file
    IO::File::Stream file;
    if (file.openRW(filename))
    {
        file << data;
        return true;
    }
    logs.error() << "impossible to write " << filename;
    return false;
}

bool AreaList::preloadAndMarkAsModifiedAllInvalidatedAreas(uint* invalidateCount) const
{
    bool ret = true;
    uint count = 0;
    each([&](const Data::Area& area) {
        if (area.invalidateJIT)
        {
            logs.info() << "Preparing the area " << area.name;
            // invalidating all data belonging to the area
            ret = area.invalidate(true) and ret;
            // marking the area as modified to force the incremental save
            area.markAsModified();
            ++count;
        }
    });
    if (invalidateCount)
        *invalidateCount = count;
    return ret;
}

void AreaList::markAsModified() const
{
    each([&](const Data::Area& area) { area.markAsModified(); });
}

bool AreaList::saveToFolder(const AnyString& folder) const
{
    if (folder.empty())
    {
        logs.error() << "areas: invalid empty target folder";
        assert(false and "invalid empty folder");
        return false;
    }

    // Temporary buffer
    Clob buffer;

    // Invalidating all areas if required
    // Forcing the loading of all data before writing something to prevent
    // agains circular area renaming.
    // Example :
    //   rename A -> B
    //   rename B -> A
    preloadAndMarkAsModifiedAllInvalidatedAreas();

    // Make sure a default structure is present
    // All sub-routines must not rely on it. It just ensures that a common
    // structure is present for the user
    bool ret = true;

    // Create the whole structure
    buffer.clear() << folder << SEP << "input" << SEP << "areas";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "reserves";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "bindingconstraints";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "links";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "series";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "series";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "series";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "series";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "allocation";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "common" << SEP
                   << "capacity";
    ret = IO::Directory::Create(buffer) and ret;

    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "series";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) and ret;
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "clusters";
    ret = IO::Directory::Create(buffer) and ret;

    // Write the list of areas to a flat file
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << "list.txt";
    ret = saveListToFile(buffer) and ret;

    // Thermal data, specific to areas
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "areas.ini";
    ret = AreaListSaveThermalDataToFile(*this, buffer) and ret;

    // Save all areas
    each([&](const Data::Area& area) {
        logs.info() << "Exporting the area " << (area.index + 1) << '/' << areas.size() << ": "
                    << area.name;
        ret = AreaListSaveToFolderSingleArea(area, buffer, folder) and ret;
    });

    // Hydro
    // The hydro files must be saved after the area has been invalidated
    buffer.clear() << folder << SEP << "input" << SEP << "hydro";
    ret = PartHydro::SaveToFolder(*this, buffer) and ret;

    // update nameid set
    updateNameIDSet();

    return ret;
}

template<class StringT>
static bool AreaListLoadFromFolderSingleArea(Study& study,
                                             AreaList* list,
                                             Area& area,
                                             StringT& buffer,
                                             const StudyLoadOptions& options)
{
    // Progression
    options.progressTicks = 0;
    options.progressTickCount
      = area.thermal.list.size() * (options.loadOnlyNeeded ? 1 : 2) // prepro+series
        + 1                                                         // links
        + 4                                                         // load,solar,wind,hydro
        + 1;                                                        // DSM,misc...

    // Reset
    area.filterSynthesis = filterAll;
    area.filterYearByYear = filterAll;

    area.nodalOptimization = 0;
    area.spreadUnsuppliedEnergyCost = 0.;
    area.spreadSpilledEnergyCost = 0.;

    bool ret = true;

    // DSM, Reserves, D-1
    if (study.header.version < 320)
    {
        area.reserves.reset(fhrMax, HOURS_PER_YEAR, true);

        // We should allocate on the heap to avoid stack overflow (Windows)
        double* tmp = new double[HOURS_PER_YEAR];

        // DSM
        buffer.clear() << study.folderInput << SEP << "reserves-dsm" << SEP << area.id << SEP
                       << "dsm." << study.inputExtension;
        ret = Array1DLoadFromFile(buffer.c_str(), tmp, HOURS_PER_YEAR) and ret;
        area.reserves.pasteToColumn(fhrDSM, tmp);

        // Strategic Reserves
        buffer.clear() << study.folderInput << SEP << "reserves-dsm" << SEP << area.id << SEP
                       << "reserves." << study.inputExtension;
        ret = Array1DLoadFromFile(buffer.c_str(), tmp, HOURS_PER_YEAR) and ret;
        buffer.clear() << "Reserves: `" << area.id << '`';
        Array1DCheckPositiveValues(buffer.c_str(), tmp, HOURS_PER_YEAR);
        area.reserves.pasteToColumn(fhrStrategicReserve, tmp);

        // Primary reserves
        buffer.clear() << study.folderInput << SEP << "reserves-dsm" << SEP << area.id << SEP
                       << "primaryreserves." << study.inputExtension;
        ret = Array1DLoadFromFile(buffer.c_str(), tmp, HOURS_PER_YEAR) and ret;
        buffer.clear() << "Primary Reserves: `" << area.id << '`';
        Array1DCheckPositiveValues(buffer.c_str(), tmp, HOURS_PER_YEAR);
        area.reserves.pasteToColumn(fhrPrimaryReserve, tmp);

        // Release
        delete[] tmp;
        // flush
        area.reserves.flush();
    }
    else
    {
        buffer.clear() << study.folderInput << SEP << "reserves" << SEP << area.id << ".txt";
        ret = area.reserves.loadFromCSVFile(buffer, fhrMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
              and ret;
    }

    // Optimzation preferences
    if (study.usedByTheSolver)
    {
        if (not study.parameters.include.reserve.dayAhead)
            area.reserves.columnToZero(fhrDayBefore);
        if (not study.parameters.include.reserve.strategic)
            area.reserves.columnToZero(fhrStrategicReserve);
        if (not study.parameters.include.reserve.primary)
            area.reserves.columnToZero(fhrPrimaryReserve);
        area.reserves.flush();
    }

    // Fatal hors hydro - Misc Gen.
    if (study.header.version <= 320)
    {
        area.solar.series->series.resize(1, HOURS_PER_YEAR);
        area.miscGen.reset(fhhMax, HOURS_PER_YEAR, true);

        buffer.clear() << study.folderInput << SEP << "misc-gen" << SEP << area.id << SEP
                       << "fatalhorshydro." << study.inputExtension;
        if (study.header.version < 210)
        {
            Matrix<double> m;
            bool r = m.loadFromCSVFile(buffer,
                                       5,
                                       8760 /* hardcoded in this version */,
                                       Matrix<>::optFixedSize | Matrix<>::optImmediate);
            ret = r and ret;
            if (r)
            {
                // Make the data available, if not already done
                m.invalidate(true);
                // Copy
                (void)memcpy(area.miscGen[fhhBioMass], m[0], (size_t)(8760 * sizeof(double)));
                (void)memcpy(area.miscGen[fhhCHP], m[2], (size_t)(8760 * sizeof(double)));
                (void)memcpy(area.miscGen[fhhOther], m[3], (size_t)(8760 * sizeof(double)));
                (void)memcpy(area.miscGen[fhhRowBalance], m[4], (size_t)(8760 * sizeof(double)));

                (void)memcpy(
                  area.solar.series->series[0], m[1], (size_t)(m.height * sizeof(double)));
                // Keep the user posted
                LogCompatibility(
                  "'%s': Misc Gen.: The data have been converted (changes from v2.1)",
                  area.id.c_str());
            }
        }
        else
        {
            Matrix<double> m;
            // CHP, Solar, BioMass, BioGaz, Waste, GeoThermal, Other, PSP, RowBalance
            bool r = m.loadFromCSVFile(buffer, 9, HOURS_PER_YEAR, Matrix<>::optFixedSize) and ret;
            ret = r and ret;
            if (r)
            {
                // Make the data available, if not already done
                m.invalidate(true);

                (void)memcpy(area.miscGen[fhhCHP], m[0], (size_t)(m.height * sizeof(double)));
                (void)memcpy(area.miscGen[fhhBioMass], m[2], (size_t)(m.height * sizeof(double)));
                (void)memcpy(area.miscGen[fhhBioGaz], m[3], (size_t)(m.height * sizeof(double)));
                (void)memcpy(area.miscGen[fhhWaste], m[4], (size_t)(m.height * sizeof(double)));
                (void)memcpy(
                  area.miscGen[fhhGeoThermal], m[5], (size_t)(m.height * sizeof(double)));
                (void)memcpy(area.miscGen[fhhOther], m[6], (size_t)(m.height * sizeof(double)));
                (void)memcpy(area.miscGen[fhhPSP], m[7], (size_t)(m.height * sizeof(double)));
                (void)memcpy(
                  area.miscGen[fhhRowBalance], m[8], (size_t)(m.height * sizeof(double)));

                (void)memcpy(
                  area.solar.series->series[0], m[1], (size_t)(m.height * sizeof(double)));
                // Keep the user posted
                LogCompatibility(
                  "'%s': Misc Gen.: The data have been converted (changes from v3.3)",
                  area.id.c_str());
            }
        }
        area.miscGen.markAsModified();
        area.solar.series->series.markAsModified();
    }
    else
    {
        // >= 330
        buffer.clear() << study.folderInput << SEP << "misc-gen" << SEP << "miscgen-" << area.id
                       << ".txt";
        ret = area.miscGen.loadFromCSVFile(buffer, fhhMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
              and ret;
    }
    // Check misc gen
    {
        buffer.clear() << "Misc Gen: `" << area.id << '`';
        MatrixTestForPositiveValues_LimitWidth(buffer.c_str(), &area.miscGen, fhhPSP);
    }
    area.miscGen.flush();

    ++options.progressTicks;
    options.pushProgressLogs();

    // Links
    {
        if (study.header.version < 350)
            buffer.clear() << study.folderInput << SEP << "interconnections" << SEP << area.id;
        else
            buffer.clear() << study.folderInput << SEP << "links" << SEP << area.id;
        ret = AreaLinksLoadFromFolder(study, list, &area, buffer) and ret;
        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // UI
    if (JIT::usedFromGUI)
    {
        if (not area.ui)
            area.ui = new AreaUI();

        buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP << "ui.ini";
        ret = area.ui->loadFromFile(buffer) and ret;
    }

    // Load
    {
        if (area.load.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "load" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.load.prepro->loadFromFolder(study, buffer) and ret;
        }
        if (area.load.series and (!options.loadOnlyNeeded or !area.load.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "load" << SEP << "series";
            ret = DataSeriesLoadLoadFromFolder(study, area.load.series, area.id, buffer.c_str())
                  and ret;
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "solar" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.solar.prepro->loadFromFolder(study, buffer) and ret;
        }
        if (area.solar.series and (!options.loadOnlyNeeded or !area.solar.prepro)) // Series
        {
            if (study.header.version >= 330)
            {
                buffer.clear() << study.folderInput << SEP << "solar" << SEP << "series";
                ret
                  = DataSeriesSolarLoadFromFolder(study, area.solar.series, area.id, buffer.c_str())
                    and ret;
            }
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Hydro
    {
        // Allocation
        buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "allocation" << SEP
                       << area.id << ".ini";
        ret = area.hydro.allocation.loadFromFile(area.id, buffer) and ret;

        if (area.hydro.prepro) /* Hydro */
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "prepro";
            ret = area.hydro.prepro->loadFromFolder(study, area.id, buffer.c_str()) and ret;
        }
        if (area.hydro.series and (!options.loadOnlyNeeded or !area.hydro.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "series";
            ret = area.hydro.series->loadFromFolder(study, area.id, buffer) and ret;
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Wind
    {
        if (area.wind.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "wind" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.wind.prepro->loadFromFolder(study, buffer) and ret;
        }
        if (area.wind.series and (!options.loadOnlyNeeded or !area.wind.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "wind" << SEP << "series";
            ret = DataSeriesWindLoadFromFolder(study, area.wind.series, area.id, buffer.c_str())
                  and ret;
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Thermal cluster list
    {
        if (not options.loadOnlyNeeded)
        {
            buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "prepro";
            ret = ThermalClusterListLoadPreproFromFolder(study, options, &area.thermal.list, buffer)
                  and ret;
            buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "series";
            ret = ThermalClusterListLoadDataSeriesFromFolder(
                    study, options, &area.thermal.list, buffer, options.loadOnlyNeeded)
                  and ret;
        }
        else
        {
            if (study.parameters.isTSGeneratedByPrepro(timeSeriesThermal))
            {
                buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "prepro";
                ret = ThermalClusterListLoadPreproFromFolder(
                        study, options, &area.thermal.list, buffer)
                      and ret;
            }
            else
            {
                buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "series";
                ret = ThermalClusterListLoadDataSeriesFromFolder(
                        study, options, &area.thermal.list, buffer, options.loadOnlyNeeded)
                      and ret;
            }
        }

        if (study.header.version < 390)
        {
            // we must use an intermediate list since the original one will be
            // altered by the rename
            std::vector<Data::ThermalCluster*> list;
            area.thermal.list.each(
              [&](Data::ThermalCluster& cluster) { list.push_back(&cluster); });

            foreach (auto* cluster, list)
            {
                // We may have some strange name/id in older studies
                // force full reloading
                cluster->invalidate(true);
                // marking the thermal plant as modified
                cluster->markAsModified();

                // applying the new naming convention
                String newname;
                BeautifyName(newname, cluster->name());
                study.thermalClusterRename(cluster, newname, true);
            }
        }

        // In adequacy mode, all thermal clusters must be in 'mustrun' mode
        if (study.usedByTheSolver and study.parameters.mode == stdmAdequacy)
            area.thermal.list.enableMustrunForEveryone();

        // flush
        area.thermal.list.flush();
    }

    // Nodal Optimization
    if (study.header.version >= 330)
    {
        buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP
                       << "optimization.ini";
        IniFile ini;
        if (ini.open(buffer))
        {
            ini.each([&](const IniFile::Section& section) {
                for (auto* p = section.firstProperty; p; p = p->next)
                {
                    bool value = p->value.to<bool>();
                    CString<30, false> tmp;
                    tmp = p->key;
                    tmp.toLower();
                    if (tmp == "non-dispatchable-power")
                    {
                        if (value)
                            area.nodalOptimization |= anoNonDispatchPower;
                        continue;
                    }
                    if (tmp == "dispatchable-hydro-power")
                    {
                        if (value)
                            area.nodalOptimization |= anoDispatchHydroPower;
                        continue;
                    }
                    if (tmp == "other-dispatchable-power")
                    {
                        if (value)
                            area.nodalOptimization |= anoOtherDispatchPower;
                        continue;
                    }
                    if (tmp == "filter-synthesis")
                    {
                        area.filterSynthesis = StringToFilter(p->value);
                        continue;
                    }
                    if (tmp == "filter-year-by-year")
                    {
                        area.filterYearByYear = StringToFilter(p->value);
                        continue;
                    }
                    if (tmp == "spread-unsupplied-energy-cost")
                    {
                        if (!p->value.to<double>(area.spreadUnsuppliedEnergyCost))
                        {
                            area.spreadUnsuppliedEnergyCost = 0.;
                            logs.warning()
                              << area.name << ": invalid spread for unsupplied energy cost";
                        }
                        continue;
                    }
                    if (tmp == "spread-spilled-energy-cost")
                    {
                        if (!p->value.to<double>(area.spreadSpilledEnergyCost))
                        {
                            area.spreadSpilledEnergyCost = 0.;
                            logs.warning()
                              << area.name << ": invalid spread for spilled energy cost";
                        }
                        continue;
                    }

                    logs.warning() << buffer << ": Unknown property '" << p->key << "'";
                }
            });
        }
    }
    else
    {
        // Enable all features at once
        area.nodalOptimization = anoAll;
    }

    if (study.header.version < 380)
    {
        // For studies older than 3.8, we have to readjust the spread for both
        // the unsupplied and spilled energy cost
        // Note : this code must be executed after that thermal clusters are
        // loaded
        area.spreadSpilledEnergyCost = 1e-6 * area.thermal.spilledEnergyCost;
        area.spreadUnsuppliedEnergyCost = 1e-6 * area.thermal.unsuppliedEnergyCost;
    }

    // MBO 15/04/2014
    // limits to spreads
    if (study.header.version < 450)
    {
        if (area.spreadSpilledEnergyCost < 5.e-3)
            area.spreadSpilledEnergyCost = 0.;
        else
        {
            if (area.spreadSpilledEnergyCost > 5.e4)
                area.spreadSpilledEnergyCost = 5.e4;
        }

        if (area.spreadUnsuppliedEnergyCost < 5.e-3)
            area.spreadUnsuppliedEnergyCost = 0.;
        else
        {
            if (area.spreadUnsuppliedEnergyCost > 5.e4)
                area.spreadUnsuppliedEnergyCost = 5.e4;
        }
    }

    return ret;
}

bool AreaList::loadFromFolder(const StudyLoadOptions& options)
{
    bool ret = true;
    Clob buffer;

    // Load the list of all available areas
    {
        logs.info() << "Loading the list of areas...";
        buffer.clear() << pStudy.folderInput << SEP << "areas" << SEP << "list."
                       << pStudy.inputExtension;
        ret = loadListFromFile(buffer) and ret;
    }

    // Hydro
    {
        logs.info() << "Loading global hydro data...";
        buffer.clear() << pStudy.folderInput << SEP << "hydro";
        ret = PartHydro::LoadFromFolder(pStudy, buffer) and ret;
    }

    // Thermal data, specific to areas
    {
        logs.info() << "Loading thermal clusters...";
        buffer.clear() << pStudy.folderInput << SEP << "thermal" << SEP << "areas.ini";
        ret = AreaListLoadThermalDataFromFile(*this, buffer) and ret;

        // The cluster list must be loaded before the method
        // Study::ensureDataAreInitializedAccordingParameters() is called
        // in order to allocate data with all thermal clusters.
        CString<30, false> thermalPlant;
        if (pStudy.header.version < 350)
            thermalPlant << SEP << "thermal" << SEP << "aggregates" << SEP;
        else
            thermalPlant << SEP << "thermal" << SEP << "clusters" << SEP;

        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            buffer.clear() << pStudy.folderInput << thermalPlant << area.id;
            ret = area.thermal.list.loadFromFolder(pStudy, buffer.c_str(), &area) and ret;
            area.thermal.prepareAreaWideIndexes();
        }
    }

    // Prepare
    if (options.loadOnlyNeeded)
        // Only data we need
        pStudy.ensureDataAreInitializedAccordingParameters();
    else
        // We want all data, without exception
        pStudy.ensureDataAreAllInitialized();

    // Load all nodes
    uint indx = 0;
    each([&](Data::Area& area) {
        // Progression
        options.logMessage.clear()
          << "Loading the area " << (++indx) << '/' << areas.size() << ": " << area.name;
        logs.info() << options.logMessage;

        // Load a single area
        ret = AreaListLoadFromFolderSingleArea(pStudy, this, area, buffer, options) and ret;
    });

    // update nameid set
    updateNameIDSet();
    return ret;
}

Area* AreaList::find(const AreaName& id)
{
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second : nullptr;
}

const Area* AreaList::find(const AreaName& id) const
{
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second : nullptr;
}

Area* AreaList::findFromName(const AreaName& name)
{
    AreaName id;
    TransformNameIntoID(name, id);
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second : nullptr;
}

Area* AreaList::findFromPosition(const int x, const int y) const
{
    if (!this->areas.empty())
    {
        auto end = this->areas.rend();
        double nearestDistance = 5;
        Area* nearestItem = nullptr;
        for (auto i = this->areas.rbegin(); i != end; ++i)
        {
            auto lastArea = i->second;
            if (lastArea->ui)
                if (Math::Abs(lastArea->ui->x - x) < nearestDistance
                    && Math::Abs(lastArea->ui->y - y) < nearestDistance)
                {
                    nearestItem = lastArea;
                }
        }
        return nearestItem;
    }
    return nullptr;
}

const Area* AreaList::findFromName(const AreaName& name) const
{
    AreaName id;
    TransformNameIntoID(name, id);
    auto i = this->areas.find(id);
    return (i != this->areas.end()) ? i->second : nullptr;
}

Area* AreaListLFind(AreaList* l, const char lname[])
{
    if (l and !l->areas.empty() and lname)
    {
        auto i = l->areas.find(AreaName(lname));
        return (i != l->areas.end()) ? i->second : nullptr;
    }
    return nullptr;
}

Area* AreaListFindPtr(AreaList* l, const Area* ptr)
{
    if (l and ptr)
    {
        auto end = l->areas.end();
        for (auto i = l->areas.begin(); i != end; ++i)
        {
            if (ptr == i->second)
                return i->second;
        }
    }
    return nullptr;
}

void AreaListEnsureDataLoadTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.load.series)
            area.load.series = new DataSeriesLoad();
    });
}

void AreaListEnsureDataLoadPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.load.prepro)
            area.load.prepro = new Antares::Data::Load::Prepro();
    });
}

void AreaListEnsureDataSolarTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.solar.series)
            area.solar.series = new DataSeriesSolar();
    });
}

void AreaListEnsureDataSolarPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.solar.prepro)
            area.solar.prepro = new Antares::Data::Solar::Prepro();
    });
}

void AreaListEnsureDataWindTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.wind.series)
            area.wind.series = new DataSeriesWind();
    });
}

void AreaListEnsureDataWindPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.wind.prepro)
            area.wind.prepro = new Antares::Data::Wind::Prepro();
    });
}

void AreaListEnsureDataHydroTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.hydro.series)
            area.hydro.series = new DataSeriesHydro();
    });
}

void AreaListEnsureDataHydroPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each([&](Data::Area& area) {
        if (!area.hydro.prepro)
            area.hydro.prepro = new PreproHydro();
    });
}

void AreaListEnsureDataThermalTimeSeries(AreaList* l)
{
    assert(l);

    l->each([&](Data::Area& area) { ThermalClusterListEnsureDataTimeSeries(&area.thermal.list); });
}

void AreaListEnsureDataThermalPrepro(AreaList* l)
{
    assert(l and "The area list must not be nullptr");

    l->each([&](Data::Area& area) { ThermalClusterListEnsureDataPrepro(&area.thermal.list); });
}

uint64 AreaList::memoryUsage() const
{
    Yuni::uint64 ret = sizeof(AreaList) + sizeof(Area**) * areas.size();
    each([&](const Data::Area& area) { ret += area.memoryUsage(); });
    return ret;
}

void AreaList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    u.requiredMemoryForInput += (sizeof(void*) * 3) * areas.size();
    each([&](const Data::Area& area) { area.estimateMemoryUsage(u); });
}

double AreaList::memoryUsageAveragePerArea() const
{
    if (!areas.empty()) // avoid division by 0
    {
        Yuni::uint64 ret = 0;
        each([&](const Data::Area& area) { ret += area.memoryUsage(); });
        return (double)((double)ret / (double)areas.size());
    }
    return 0;
}

uint AreaList::areaLinkCount() const
{
    uint ret = 0;
    each([&](const Data::Area& area) { ret += (uint)area.links.size(); });
    return ret;
}

void Area::detachLinkFromItsPointer(const AreaLink* lnk)
{
    auto end = links.end();
    for (auto i = links.begin(); i != end; ++i)
    {
        if (i->second == lnk)
        {
            links.erase(i);
            return;
        }
    }
}

bool AreaList::renameArea(const AreaName& oldid, const AreaName& newName)
{
    AreaName newid;
    TransformNameIntoID(newName, newid);
    return renameArea(oldid, newid, newName);
}

bool AreaList::renameArea(const AreaName& oldid, const AreaName& newid, const AreaName& newName)
{
    if (!oldid or !newName or !newid or areas.empty())
        return false;

    // Detaching the area from the list
    Area* area;
    {
        auto i = areas.find(oldid);
        if (i == areas.end())
            return false;
        area = i->second;
        areas.erase(i);
    }

    if (find(newid))
    {
        // Another area with the same ID already exists
        // Aborting.
        areas[newid] = area;
        return false;
    }

    // Renaming the area
    area->id = newid;
    area->name = newName;

    area->invalidateJIT = true;
    areas[area->id] = area;

    // We have to update all links connected to this area
    each([&](Data::Area& a) {
        auto* link = a.findLinkByID(oldid);
        if (!link)
            return;

#ifndef NDEBUG
        uint oldCount = (uint)a.links.size();
#endif
        // Renamming the entry

        link->invalidate(true);
        link->markAsModified();

        link->detach();
        a.links[link->with->id] = link;

#ifndef NDEBUG
        assert(oldCount == a.links.size() and "We must have the same number of items in the list");
#endif
    });

    return true;
}

void AreaListDeleteLinkFromAreaPtr(AreaList* list, const Area* a)
{
    if (!list or !a)
        return;

    list->each([&](Data::Area& area) {
        if (not area.links.empty())
            return;
        bool mustLoop = false;
        do
        {
            mustLoop = false;
            // Foreach link from this area
            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
            {
                AreaLink* lnk = i->second;

                // The link must be destroyed if attached to the given area
                if ((lnk->from == a) or (lnk->with == a))
                {
                    // The reference to this link will be removed and the link will be freed
                    AreaLinkRemove(lnk);
                    // Let's start again
                    mustLoop = true;
                    break;
                }
            }
        } while (mustLoop);
    });
}

bool AreaList::invalidate(bool reload) const
{
    bool ret = true;
    each([&](Data::Area& area) { ret = area.invalidate(reload) and ret; });
    return ret;
}

void AreaList::resizeAllTimeseriesNumbers(uint n)
{
    // Ask to resize the matrices dedicated to the sampled timeseries numbers
    // for each area
    each([&](Data::Area& area) { area.resizeAllTimeseriesNumbers(n); });
}

void AreaList::fixOrientationForAllInterconnections(BindConstList& bindingconstraints)
{
    each([&](Data::Area& area) {
        bool mustLoop;
        // for each link from this area
        do
        {
            // Nothing to do if the area does not have any links
            if (area.links.empty())
                break;

            // By default, we don't have to loop forever
            mustLoop = false;

            // Foreach link...
            auto end = area.links.end();
            for (auto i = area.links.begin(); i != end; ++i)
            {
                // Reference to the link
                auto& link = *(i->second);
                // Asserts
                assert(&link);
                assert(link.from);
                assert(link.with);

                if ((link.from)->id > (link.with)->id)
                {
                    // Reversing the link
                    link.reverse();
                    // Updating the binding constraints
                    bindingconstraints.reverseWeightSign(&link);
                    // Since the iterators have been compromised, we have to restart the iteration
                    // through the links
                    mustLoop = true;
                    break;
                }
            }
        } while (mustLoop);
    });
}

bool AreaList::remove(const AnyString& id)
{
    AreaName lname;
    TransformNameIntoID(id, lname);

    auto i = areas.find(lname);
    if (i != areas.end())
    {
        // Referene to the area
        auto* areaToRemove = i->second;

        // We remove all links starting from this node in a first time
        areaToRemove->detachAllLinks();
        // Then we remove the reference to this area
        areas.erase(i);

        // All dependencies must be removed as well
        AreaListDeleteLinkFromAreaPtr(this, areaToRemove);

        // Finally we can destroy the area itself
        delete areaToRemove;

        return true;
    }
    return false;
}

AreaLink* AreaList::findLinkFromINIKey(const AnyString& key)
{
    if (key.empty())
        return nullptr;
    auto offset = key.find('%');
    if (offset == AreaName::npos or (0 == offset) or (offset == key.size() - 1))
        return nullptr;
    AreaName from(key.c_str(), offset);
    AreaName to(key.c_str() + offset + 1, key.size() - (offset + 1));

    return findLink(from, to);
}

ThermalCluster* AreaList::findClusterFromINIKey(const AnyString& key)
{
    if (key.empty())
        return nullptr;
    auto offset = key.find('.');
    if (offset == AreaName::npos or (0 == offset) or (offset == key.size() - 1))
        return nullptr;
    AreaName parentName(key.c_str(), offset);
    ThermalClusterName id(key.c_str() + offset + 1, key.size() - (offset + 1));
    Area* parentArea = findFromName(parentName);
    if (parentArea == nullptr)
        return nullptr;
    ThermalCluster* i = parentArea->thermal.list.find(id);
    return (i != nullptr) ? i : nullptr;
}

void AreaList::updateNameIDSet() const
{
    nameidSet.clear();
    auto end = areas.end();
    for (auto i = areas.begin(); i != end; ++i)
    {
        auto& area = *(i->second);
        nameidSet.insert(area.id);
    }
}

void AreaList::removeLoadTimeseries()
{
    each([&](Data::Area& area) { area.load.series->series.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeHydroTimeseries()
{
    each([&](Data::Area& area) {
        area.hydro.series->ror.reset(1, HOURS_PER_YEAR);
        area.hydro.series->storage.reset(1, DAYS_PER_YEAR);
        area.hydro.series->count = 1;
    });
}

void AreaList::removeSolarTimeseries()
{
    each([&](Data::Area& area) { area.solar.series->series.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeWindTimeseries()
{
    each([&](Data::Area& area) { area.wind.series->series.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeThermalTimeseries()
{
    each([&](Data::Area& area) {
        area.thermal.list.each(
          [&](Data::ThermalCluster& cluster) { cluster.series->series.reset(1, HOURS_PER_YEAR); });
    });
}

} // namespace Data
} // namespace Antares
