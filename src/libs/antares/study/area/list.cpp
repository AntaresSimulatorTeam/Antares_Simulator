/*
** Copyright 2007-2023 RTE
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
#include "../study.h"
#include <cassert>
#include "area.h"
#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include "constants.h"
#include "antares/study/parts/parts.h"
#include "antares/study/parts/load/prepro.h"

#define SEP IO::Separator

using namespace Yuni;

namespace Antares::Data
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
    if (!ini.open(filename))
        return false;

    auto* section = ini.find("unserverdenergycost");

    // Try to find the section
    if (section && section->firstProperty)
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
    if (section && section->firstProperty)
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
    IniFile ini;

    IniFile::Section* s = ini.addSection("unserverdenergycost");
    list.each([&](const Data::Area& area) {
        // 0 values are skipped
        if (!Math::Zero(area.thermal.unsuppliedEnergyCost))
            s->add(area.id, area.thermal.unsuppliedEnergyCost);
    });

    s = ini.addSection("spilledenergycost");
    list.each([&](const Data::Area& area) {
        // 0 values are skipped
        if (!Math::Zero(area.thermal.spilledEnergyCost))
            s->add(area.id, area.thermal.spilledEnergyCost);
    });

    return ini.save(filename);
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
    if (area.ui && area.ui->modified())
    {
        buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id << SEP
                       << "ui.ini";
        ret = area.ui->saveToFile(buffer) && ret;
    }

    // Nodal optimization
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id << SEP
                   << "optimization.ini";
    ret = saveAreaOptimisationIniFile(area, buffer) && ret;

    // Adequacy ini
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << area.id << SEP
                   << "adequacy_patch.ini";
    ret = saveAreaAdequacyPatchIniFile(area, buffer) && ret;

    // Reserves: primary, strategic, dsm, d-1...
    buffer.clear() << folder << SEP << "input" << SEP << "reserves" << SEP << area.id << ".txt";
    ret = area.reserves.saveToCSVFile(buffer) && ret;

    // Fatal hors hydro
    buffer.clear() << folder << SEP << "input" << SEP << "misc-gen" << SEP << "miscgen-" << area.id
                   << ".txt";
    ret = area.miscGen.saveToCSVFile(buffer) && ret;

    // NTC
    buffer.clear() << folder << SEP << "input" << SEP << "links" << SEP << area.id;
    ret = AreaLinksSaveToFolder(&area, buffer.c_str()) && ret;

    // Load
    {
        if (area.load.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.load.prepro->saveToFolder(buffer) && ret;
        }
        if (area.load.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "series";
            ret = DataSeriesLoadSaveToFolder(area.load.series, area.id, buffer.c_str()) && ret;
        }
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.solar.prepro->saveToFolder(buffer) && ret;
        }
        if (area.solar.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "series";
            ret = DataSeriesSolarSaveToFolder(area.solar.series, area.id, buffer.c_str()) && ret;
        }
    }

    // Hydro
    {
        buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "allocation" << SEP
                       << area.id << ".ini";
        ret = area.hydro.allocation.saveToFile(buffer) && ret;

        if (area.hydro.prepro) // Hydro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "prepro";
            ret = area.hydro.prepro->saveToFolder(area.id, buffer.c_str()) && ret;
        }
        if (area.hydro.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "series";
            ret = area.hydro.series->saveToFolder(area.id, buffer) && ret;
        }
    }

    // Wind
    {
        if (area.wind.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.wind.prepro->saveToFolder(buffer) && ret;
        }
        if (area.wind.series) // Series
        {
            buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "series";
            ret = DataSeriesWindSaveToFolder(area.wind.series, area.id, buffer.c_str()) && ret;
        }
    }

    // Thermal cluster list
    {
        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "clusters" << SEP
                       << area.id;
        ret = area.thermal.list.saveToFolder(buffer) && ret;

        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "prepro";
        ret = area.thermal.list.savePreproToFolder(buffer) && ret;
        buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "series";
        ret = area.thermal.list.saveDataSeriesToFolder(buffer) && ret;
        ret = area.thermal.list.saveEconomicCosts(buffer) && ret;
    }

    // Renewable cluster list
    {
        buffer.clear() << folder << SEP << "input" << SEP << "renewables" << SEP << "clusters"
                       << SEP << area.id;
        ret = area.renewable.list.saveToFolder(buffer) && ret;

        buffer.clear() << folder << SEP << "input" << SEP << "renewables" << SEP << "series";
        ret = area.renewable.list.saveDataSeriesToFolder(buffer) && ret;
    }

    // Short term storage

    // save sts in list.ini for this area
    buffer.clear() << folder << SEP << "input" << SEP << "st-storage" << SEP << "clusters"
        << SEP << area.id;
    ret = area.shortTermStorage.saveToFolder(buffer.c_str()) && ret;

    // save the series files
    buffer.clear() << folder << SEP << "input" << SEP << "st-storage" << SEP << "series"
        << SEP << area.id;
    ret = area.shortTermStorage.saveDataSeriesToFolder(buffer.c_str()) && ret;

    return ret;
}

} // anonymous namespace

bool saveAreaOptimisationIniFile(const Area& area, const Clob& buffer)
{
    IniFile ini;
    IniFile::Section* section = ini.addSection("nodal optimization");

    section->add("non-dispatchable-power",
                 static_cast<bool>(area.nodalOptimization & anoNonDispatchPower));
    section->add("dispatchable-hydro-power",
                 static_cast<bool>(area.nodalOptimization & anoDispatchHydroPower));
    section->add("other-dispatchable-power",
                 static_cast<bool>(area.nodalOptimization & anoOtherDispatchPower));
    section->add("spread-unsupplied-energy-cost", area.spreadUnsuppliedEnergyCost);
    section->add("spread-spilled-energy-cost", area.spreadSpilledEnergyCost);

    section = ini.addSection("filtering");
    section->add("filter-synthesis", datePrecisionIntoString(area.filterSynthesis));
    section->add("filter-year-by-year", datePrecisionIntoString(area.filterYearByYear));

    return ini.save(buffer);
}

bool saveAreaAdequacyPatchIniFile(const Area& area, const Clob& buffer)
{
    IniFile ini;
    IniFile::Section* section = ini.addSection("adequacy-patch");
    std::string value;
    switch (area.adequacyPatchMode)
    {
    case Data::AdequacyPatch::virtualArea:
        value = "virtual";
        break;
    case Data::AdequacyPatch::physicalAreaOutsideAdqPatch:
        value = "outside";
        break;
    case Data::AdequacyPatch::physicalAreaInsideAdqPatch:
        value = "inside";
        break;
    default:
        value = "outside"; // default physicalAreaOutsideAdqPatch
        break;
    }
    section->add("adequacy-patch-mode", value);
    return ini.save(buffer);
}

AreaList::AreaList(Study& study) : byIndex(nullptr), pStudy(study)
{
}

AreaList::~AreaList()
{
    clear();
}

bool AreaList::empty() const
{
    return areas.empty();
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
            if (b && !a->findExistingLinkWith(*b))
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

    if (!areas.empty())
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
        using AreaWeakPtr = Area*;
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
Area* addAreaToListOfAreas(AreaList& list, const AnyString& name)
{
    // Initializing names
    AreaName cname;
    AreaName lname;
    cname = name;
    TransformNameIntoID(cname, lname);

    // Add the area
    return AreaListAddFromNames(list, cname, lname);
}

Area* AreaListAddFromNames(AreaList& list,
                           const AnyString& name,
                           const AnyString& lname)
{
    if (!name || !lname)
        return nullptr;
    // Look up
    if (!AreaListLFind(&list, lname.c_str()))
    {
        Area* area = new Area(name, lname);
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

        lname.clear();
        TransformNameIntoID(name, lname);
        if (lname.empty())
        {
            logs.warning() << "ignoring invalid area name: `" << name << "`, " << filename
                           << ": line " << line;
            continue;
        }
        if (CheckForbiddenCharacterInAreaName(name))
        {
            logs.error() << "character '*' is forbidden in area name: `" << name << "`";
            continue;
        }
        // Add the area in the list
        AreaListAddFromNames(*this, name, lname);
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

void AreaList::saveLinkListToBuffer(Yuni::Clob& buffer) const
{
    each([&](const Data::Area& area) {
        buffer << area.id << '\n';
        auto end = area.links.end();
        for (auto i = area.links.begin(); i != end; ++i)
            buffer << '\t' << (i->second)->with->id << '\n';
    });
}

bool AreaList::saveListToFile(const AnyString& filename) const
{
    if (!filename)
        return false;

    Clob data;
    {
        // Preparing a new list of areas, sorted by their name
        using List = std::list<std::string>;
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
            ret = area.forceReload(true) && ret;
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
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "reserves";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "bindingconstraints";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "links";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "series";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "series";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "series";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "series";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "allocation";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "hydro" << SEP << "common" << SEP
                   << "capacity";
    ret = IO::Directory::Create(buffer) && ret;

    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "series";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "prepro";
    ret = IO::Directory::Create(buffer) && ret;
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "clusters";
    ret = IO::Directory::Create(buffer) && ret;

    // Write the list of areas to a flat file
    buffer.clear() << folder << SEP << "input" << SEP << "areas" << SEP << "list.txt";
    ret = saveListToFile(buffer) && ret;

    // Thermal data, specific to areas
    buffer.clear() << folder << SEP << "input" << SEP << "thermal" << SEP << "areas.ini";
    ret = AreaListSaveThermalDataToFile(*this, buffer) && ret;

    // Save all areas
    each([&](const Data::Area& area) {
        logs.info() << "Exporting the area " << (area.index + 1) << '/' << areas.size() << ": "
                    << area.name;
        ret = AreaListSaveToFolderSingleArea(area, buffer, folder) && ret;
    });

    // Hydro
    // The hydro files must be saved after the area has been invalidated
    buffer.clear() << folder << SEP << "input" << SEP << "hydro";
    ret = PartHydro::SaveToFolder(*this, buffer) && ret;

    // update nameid set
    updateNameIDSet();

    return ret;
}

template<class StringT>
static void readAdqPatchMode(Study& study, Area& area, StringT& buffer)
{
    if (study.header.version >= 830)
    {
        buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP
                       << "adequacy_patch.ini";
        IniFile ini;
        if (ini.open(buffer))
        {
            auto* section = ini.find("adequacy-patch");
            for (auto* p = section->firstProperty; p; p = p->next)
            {
                CString<30, false> tmp;
                tmp = p->key;
                tmp.toLower();
                if (tmp == "adequacy-patch-mode")
                {
                    auto value = (p->value).toLower();

                    if (value == "virtual")
                        area.adequacyPatchMode = Data::AdequacyPatch::virtualArea;
                    else if (value == "inside")
                        area.adequacyPatchMode = Data::AdequacyPatch::physicalAreaInsideAdqPatch;
                    else
                        area.adequacyPatchMode = Data::AdequacyPatch::physicalAreaOutsideAdqPatch;
                }
            }
        }
    }
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
    buffer.clear() << study.folderInput << SEP << "reserves" << SEP << area.id << ".txt";
    ret = area.reserves.loadFromCSVFile(buffer, fhrMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
          && ret;

    // Optimzation preferences
    if (study.usedByTheSolver)
    {
        if (!study.parameters.include.reserve.dayAhead)
            area.reserves.columnToZero(fhrDayBefore);
        if (!study.parameters.include.reserve.strategic)
            area.reserves.columnToZero(fhrStrategicReserve);
        if (!study.parameters.include.reserve.primary)
            area.reserves.columnToZero(fhrPrimaryReserve);
    }

    // Fatal hors hydro - Misc Gen.
    buffer.clear() << study.folderInput << SEP << "misc-gen" << SEP << "miscgen-" << area.id
                   << ".txt";
    ret = area.miscGen.loadFromCSVFile(buffer, fhhMax, HOURS_PER_YEAR, Matrix<>::optFixedSize)
          && ret;

    // Check misc gen
    {
        buffer.clear() << "Misc Gen: `" << area.id << '`';
        MatrixTestForPositiveValues_LimitWidth(buffer.c_str(), &area.miscGen, fhhPSP);
    }

    ++options.progressTicks;
    options.pushProgressLogs();

    // Links
    {
        buffer.clear() << study.folderInput << SEP << "links" << SEP << area.id;
        ret = AreaLinksLoadFromFolder(study, list, &area, buffer) && ret;
        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // UI
    if (JIT::usedFromGUI)
    {
        if (!area.ui)
            area.ui = new AreaUI();

        buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP << "ui.ini";
        ret = area.ui->loadFromFile(buffer) && ret;
    }

    // Load
    {
        if (area.load.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "load" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.load.prepro->loadFromFolder(buffer) && ret;
        }
        if (area.load.series && (!options.loadOnlyNeeded || !area.load.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "load" << SEP << "series";
            ret = DataSeriesLoadLoadFromFolder(study, area.load.series, area.id, buffer.c_str())
                  && ret;
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
            ret = area.solar.prepro->loadFromFolder(buffer) && ret;
        }
        if (area.solar.series && (!options.loadOnlyNeeded || !area.solar.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "solar" << SEP << "series";
            ret = DataSeriesSolarLoadFromFolder(study, area.solar.series, area.id, buffer.c_str())
                && ret;
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Hydro
    {
        // Allocation
        buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "allocation" << SEP
                       << area.id << ".ini";
        ret = area.hydro.allocation.loadFromFile(area.id, buffer) && ret;

        if (area.hydro.prepro) /* Hydro */
        {
            // if changes are required, please update reloadXCastData()
            buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "prepro";
            ret = area.hydro.prepro->loadFromFolder(study, area.id, buffer.c_str()) && ret;
        }
        if (area.hydro.series && (!options.loadOnlyNeeded || !area.hydro.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "series";
            ret = area.hydro.series->loadFromFolder(study, area.id, buffer) && ret;
        }

        if (area.hydro.series && study.header.version < 870)
        {
            buffer.clear() << study.folderInput << SEP << "hydro";

            HydroMaxTimeSeriesReader reader;
            ret = reader(buffer, area) && ret;
        }

        if (area.hydro.series && study.header.version >= 870)
        {
            buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "series";
            ret = area.hydro.series->LoadMaxPower(area.id, buffer) && ret;

            if (study.usedByTheSolver)
            {
                area.hydro.series->setNbTimeSeriesSup();
                ret = area.hydro.series->postProcessMaxPowerTS(area) && ret;
                area.hydro.series->setMaxPowerTSWhenDeratedMode(study);
            }
            else
                area.hydro.series->setHydroModulability(study, area.id);
        }

        buffer.clear() << study.folderInput << SEP << "hydro" << SEP << "common" << SEP
                       << "capacity" << SEP << "maxpower_" << area.id << ".txt";

        if (bool exists = IO::File::Exists(buffer); study.header.version >= 870 && exists)
        {
            IO::File::Delete(buffer);
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
            ret = area.wind.prepro->loadFromFolder(buffer) && ret;
        }
        if (area.wind.series && (!options.loadOnlyNeeded || !area.wind.prepro)) // Series
        {
            buffer.clear() << study.folderInput << SEP << "wind" << SEP << "series";
            ret = DataSeriesWindLoadFromFolder(study, area.wind.series, area.id, buffer.c_str())
                  && ret;
        }

        ++options.progressTicks;
        options.pushProgressLogs();
    }

    // Thermal cluster list
    {
        buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "prepro";
        ret = area.thermal.list.loadPreproFromFolder(study, options, buffer) && ret;
        buffer.clear() << study.folderInput << SEP << "thermal" << SEP << "series";
        ret = area.thermal.list.loadDataSeriesFromFolder(study, options, buffer) && ret;
        ret = area.thermal.list.loadEconomicCosts(study, buffer) && ret;

        // In adequacy mode, all thermal clusters must be in 'mustrun' mode
        if (study.usedByTheSolver && study.parameters.mode == stdmAdequacy)
            area.thermal.list.enableMustrunForEveryone();
    }

    // Short term storage
    if (study.header.version >= 860)
    {
        buffer.clear() << study.folderInput << SEP << "st-storage" << SEP << "series"
            << SEP << area.id;

        ret = area.shortTermStorage.loadSeriesFromFolder(buffer.c_str()) && ret;
        ret = area.shortTermStorage.validate() && ret;
    }

    // Renewable cluster list
    if (study.header.version >= 810)
    {
        buffer.clear() << study.folderInput << SEP << "renewables" << SEP << "series";
        ret = area.renewable.list.loadDataSeriesFromFolder(study, options, buffer) && ret;
    }

    // Adequacy patch
    readAdqPatchMode(study, area, buffer);

    // Nodal Optimization
    buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP
                   << "optimization.ini";
    IniFile ini;
    if (!ini.open(buffer))
        return false;

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
                area.filterSynthesis = stringIntoDatePrecision(p->value);
                continue;
            }
            if (tmp == "filter-year-by-year")
            {
                area.filterYearByYear = stringIntoDatePrecision(p->value);
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

    return ret;
}

void AreaList::ensureDataIsInitialized(Parameters& params, bool loadOnlyNeeded)
{
    AreaListEnsureDataLoadTimeSeries(this);
    AreaListEnsureDataSolarTimeSeries(this);
    AreaListEnsureDataWindTimeSeries(this);
    AreaListEnsureDataHydroTimeSeries(this);
    AreaListEnsureDataThermalTimeSeries(this);
    AreaListEnsureDataRenewableTimeSeries(this);

    if (loadOnlyNeeded)
    {
        // Load
        if (params.isTSGeneratedByPrepro(timeSeriesLoad))
            AreaListEnsureDataLoadPrepro(this);
        // Solar
        if (params.isTSGeneratedByPrepro(timeSeriesSolar))
            AreaListEnsureDataSolarPrepro(this);
        // Hydro
        if (params.isTSGeneratedByPrepro(timeSeriesHydro))
            AreaListEnsureDataHydroPrepro(this);
        // Wind
        if (params.isTSGeneratedByPrepro(timeSeriesWind))
            AreaListEnsureDataWindPrepro(this);
    }
    else
    {
        AreaListEnsureDataLoadPrepro(this);
        AreaListEnsureDataSolarPrepro(this);
        AreaListEnsureDataHydroPrepro(this);
        AreaListEnsureDataWindPrepro(this);
    }

    // Thermal
    AreaListEnsureDataThermalPrepro(this);
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
        ret = loadListFromFile(buffer) && ret;
    }

    // Hydro
    {
        logs.info() << "Loading global hydro data...";
        buffer.clear() << pStudy.folderInput << SEP << "hydro";
        ret = PartHydro::LoadFromFolder(pStudy, buffer) && ret;
    }

    // Thermal data, specific to areas
    {
        logs.info() << "Loading thermal clusters...";
        buffer.clear() << pStudy.folderInput << SEP << "thermal" << SEP << "areas.ini";
        ret = AreaListLoadThermalDataFromFile(*this, buffer) && ret;

        // The cluster list must be loaded before the method
        // ensureDataIsInitialized is called
        // in order to allocate data with all thermal clusters.
        CString<30, false> thermalPlant;
        thermalPlant << SEP << "thermal" << SEP << "clusters" << SEP;

        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            buffer.clear() << pStudy.folderInput << thermalPlant << area.id;
            ret = area.thermal.list.loadFromFolder(pStudy, buffer.c_str(), &area) && ret;
            area.thermal.prepareAreaWideIndexes();
        }
    }

    // Short term storage data, specific to areas
    if (pStudy.header.version >= 860)
    {
        logs.info() << "Loading short term storage clusters...";
        buffer.clear() << pStudy.folderInput << SEP << "st-storage";

        if (IO::Directory::Exists(buffer))
        {
            for (const auto& [id, area] : areas)
            {
                buffer.clear() << pStudy.folderInput << SEP << "st-storage" << SEP << "clusters" << SEP << area->id;
                ret = area->shortTermStorage.createSTStorageClustersFromIniFile(buffer.c_str())
                      && ret;
            }
        }
        else
        {
            logs.info() << "Short term storage not found, skipping";
        }
    }

    // Renewable data, specific to areas
    if (pStudy.header.version >= 810)
    {
        // The cluster list must be loaded before the method
        // ensureDataIsInitialized is called
        // in order to allocate data with all renewable clusters.
        CString<30, false> renewablePlant;
        renewablePlant << SEP << "renewables" << SEP << "clusters" << SEP;

        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            buffer.clear() << pStudy.folderInput << renewablePlant << area.id;
            ret = area.renewable.list.loadFromFolder(buffer.c_str(), &area) && ret;
            area.renewable.prepareAreaWideIndexes();
        }
    }

    // Prepare
    ensureDataIsInitialized(pStudy.parameters, options.loadOnlyNeeded);

    // Load all nodes
    uint indx = 0;
    each([&](Data::Area& area) {
        // Progression
        options.logMessage.clear()
          << "Loading the area " << (++indx) << '/' << areas.size() << ": " << area.name;
        logs.info() << options.logMessage;

        // Load a single area
        ret = AreaListLoadFromFolderSingleArea(pStudy, this, area, buffer, options) && ret;
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
    if (l && !l->areas.empty() && lname)
    {
        auto i = l->areas.find(AreaName(lname));
        return (i != l->areas.end()) ? i->second : nullptr;
    }
    return nullptr;
}

Area* AreaListFindPtr(AreaList* l, const Area* ptr)
{
    if (l && ptr)
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
    l->each([&](Data::Area& area) { area.thermal.list.ensureDataTimeSeries(); });
}

void AreaListEnsureDataRenewableTimeSeries(AreaList* l)
{
    assert(l);
    l->each([&](Data::Area& area) { area.renewable.list.ensureDataTimeSeries(); });
}

void AreaListEnsureDataThermalPrepro(AreaList* l)
{
    assert(l && "The area list must not be nullptr");
    l->each([&](Data::Area& area) { area.thermal.list.ensureDataPrepro(); });
}

uint64_t AreaList::memoryUsage() const
{
    uint64_t ret = sizeof(AreaList) + sizeof(Area**) * areas.size();
    each([&](const Data::Area& area) { ret += area.memoryUsage(); });
    return ret;
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
    if (!oldid || !newName || !newid || areas.empty())
        return false;

    if (CheckForbiddenCharacterInAreaName(newName))
    {
        logs.error() << "character '*' is forbidden in area name: `" << newName << "`";
        return false;
    }
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
        // Renaming the entry

        link->forceReload(true);
        link->markAsModified();

        link->detach();
        a.links[link->with->id] = link;

#ifndef NDEBUG
        assert(oldCount == a.links.size() && "We must have the same number of items in the list");
#endif
    });

    area->buildLinksIndexes();

    return true;
}

void AreaListDeleteLinkFromAreaPtr(AreaList* list, const Area* a)
{
    if (!list || !a)
        return;

    list->each([&](Data::Area& area) {
        if (!area.links.empty())
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
                if ((lnk->from == a) || (lnk->with == a))
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

bool AreaList::forceReload(bool reload) const
{
    bool ret = true;
    each([&ret, &reload](const Data::Area& area) { ret = area.forceReload(reload) && ret; });
    return ret;
}

void AreaList::resizeAllTimeseriesNumbers(uint n)
{
    // Ask to resize the matrices dedicated to the sampled timeseries numbers
    // for each area
    each([&](Data::Area& area) { area.resizeAllTimeseriesNumbers(n); });
}

void AreaList::fixOrientationForAllInterconnections(BindingConstraintsRepository& bindingconstraints)
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
    if (offset == AreaName::npos || (0 == offset) || (offset == key.size() - 1))
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
    if (offset == AreaName::npos || (0 == offset) || (offset == key.size() - 1))
        return nullptr;
    AreaName parentName(key.c_str(), offset);
    ClusterName id(key.c_str() + offset + 1, key.size() - (offset + 1));
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
    each([&](Data::Area& area) { area.load.series->timeSeries.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeHydroTimeseries()
{
    each([&](Data::Area& area) {
        area.hydro.series->ror.reset(1, HOURS_PER_YEAR);
        area.hydro.series->storage.reset(1, DAYS_PER_YEAR);
        area.hydro.series->mingen.reset(1, HOURS_PER_YEAR);
        area.hydro.series->maxHourlyGenPower.reset(1, HOURS_PER_YEAR);
        area.hydro.series->maxHourlyPumpPower.reset(1, HOURS_PER_YEAR);
        area.hydro.series->count = 1;
        area.hydro.series->nbTimeSeriesSup = 1;
    });
}

void AreaList::removeSolarTimeseries()
{
    each([&](Data::Area& area) { area.solar.series->timeSeries.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeWindTimeseries()
{
    each([&](Data::Area& area) { area.wind.series->timeSeries.reset(1, HOURS_PER_YEAR); });
}

void AreaList::removeThermalTimeseries()
{
    each([&](Data::Area& area) {
        area.thermal.list.each(
          [&](Data::ThermalCluster& cluster) { cluster.series->timeSeries.reset(1, HOURS_PER_YEAR); });
    });
}

} // namespace Antares::Data

