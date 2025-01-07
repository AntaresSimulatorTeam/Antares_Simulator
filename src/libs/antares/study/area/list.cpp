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

#include <cassert>
#include <fstream>

#include <yuni/io/file.h>

#include <antares/inifile/inifile.h>
#include <antares/logs/logs.h>
#include <antares/study/area/scratchpad.h>
#include "antares/antares/antares.h"
#include "antares/study//study.h"
#include "antares/study/area/area.h"
#include "antares/study/parts/load/prepro.h"
#include "antares/study/parts/parts.h"
#include "antares/utils/utils.h"

#define SEP IO::Separator

using namespace Yuni;

namespace fs = std::filesystem;

namespace Antares::Data
{
namespace // anonymous
{
static bool AreaListLoadThermalDataFromFile(AreaList& list, const fs::path& filename)
{
    // Reset to 0
    list.each(
      [](Data::Area& area)
      {
          area.thermal.unsuppliedEnergyCost = 0.;
          area.thermal.spilledEnergyCost = 0.;
      });

    IniFile ini;
    // Try to load the file
    if (!ini.open(filename))
    {
        return false;
    }

    auto* section = ini.find("unserverdenergycost");

    // Try to find the section
    if (section && section->firstProperty)
    {
        AreaName id;
        for (IniFile::Property* p = section->firstProperty; p; p = p->next)
        {
            id.clear();
            id = transformNameIntoID(p->key);
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
                if (std::abs(area->thermal.unsuppliedEnergyCost) < 5.e-3)
                {
                    area->thermal.unsuppliedEnergyCost = 0.;
                }
                else
                {
                    if (area->thermal.unsuppliedEnergyCost > 5.e4)
                    {
                        area->thermal.unsuppliedEnergyCost = 5.e4;
                    }
                    else
                    {
                        if (area->thermal.unsuppliedEnergyCost < -5.e4)
                        {
                            area->thermal.unsuppliedEnergyCost = -5.e4;
                        }
                    }
                }
            }
            else
            {
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
            }
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
            id = transformNameIntoID(p->key);
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
                if (std::abs(area->thermal.spilledEnergyCost) < 5.e-3)
                {
                    area->thermal.spilledEnergyCost = 0.;
                }
                else
                {
                    if (area->thermal.spilledEnergyCost > 5.e4)
                    {
                        area->thermal.spilledEnergyCost = 5.e4;
                    }
                    else
                    {
                        if (area->thermal.spilledEnergyCost < -5.e4)
                        {
                            area->thermal.spilledEnergyCost = -5.e4;
                        }
                    }
                }
            }
            else
            {
                logs.warning() << filename << ": `" << p->key << "`: Unknown area";
            }
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
    list.each(
      [&s](const Data::Area& area)
      {
          // 0 values are skipped
          if (!Utils::isZero(area.thermal.unsuppliedEnergyCost))
          {
              s->add(area.id, area.thermal.unsuppliedEnergyCost);
          }
      });

    s = ini.addSection("spilledenergycost");
    list.each(
      [&s](const Data::Area& area)
      {
          // 0 values are skipped
          if (!Utils::isZero(area.thermal.spilledEnergyCost))
          {
              s->add(area.id, area.thermal.spilledEnergyCost);
          }
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
        buffer.clear() << folder << SEP << "input" << SEP << "load" << SEP << "series";
        area.load.series.saveToFolder(area.id, buffer.c_str(), "load_") && ret;
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "prepro" << SEP
                           << area.id;
            ret = area.solar.prepro->saveToFolder(buffer) && ret;
        }
        buffer.clear() << folder << SEP << "input" << SEP << "solar" << SEP << "series";
        ret = area.solar.series.saveToFolder(area.id, buffer.c_str(), "solar_") && ret;
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

        buffer.clear() << folder << SEP << "input" << SEP << "wind" << SEP << "series";
        ret = area.wind.series.saveToFolder(area.id, buffer.c_str(), "wind_") && ret;
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
    buffer.clear() << folder << SEP << "input" << SEP << "st-storage" << SEP << "clusters" << SEP
                   << area.id;
    ret = area.shortTermStorage.saveToFolder(buffer.c_str()) && ret;

    // save the series files
    buffer.clear() << folder << SEP << "input" << SEP << "st-storage" << SEP << "series" << SEP
                   << area.id;
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

AreaList::AreaList(Study& study):
    pStudy(study)
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

        AreaName givenName = area;
        AreaName name = transformNameIntoID(givenName);
        Area* a = AreaListLFind(l, name.c_str());
        if (a)
        {
            givenName = with;
            name.clear();
            name = transformNameIntoID(givenName);
            Area* b = l->find(name);
            if (b && !a->findExistingLinkWith(*b))
            {
                return AreaAddLinkBetweenAreas(a, b, warning);
            }
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
        {
            return (*(i->second)).findExistingLinkWith(*(j->second));
        }
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
        {
            return (*(i->second)).findExistingLinkWith(*(j->second));
        }
    }
    return nullptr;
}

void AreaList::clear()
{
    byIndex.clear();

    Area::Map copy;
    copy.swap(areas);

    auto end = copy.end();
    for (auto i = copy.begin(); i != end; ++i)
    {
        delete i->second;
    }
}

void AreaList::rebuildIndexes()
{
    byIndex.clear();

    byIndex.resize(areas.size());

    uint indx = 0;
    auto end = areas.end();
    for (auto i = areas.begin(); i != end; ++i, ++indx)
    {
        Area* area = i->second;
        byIndex[indx] = area;
        area->index = indx;
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
    AreaName lname = transformNameIntoID(name);

    // Add the area
    return AreaListAddFromNames(list, name, lname);
}

Area* AreaListAddFromNames(AreaList& list, const AnyString& name, const AnyString& lname)
{
    if (!name || !lname)
    {
        return nullptr;
    }
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
    {
        logs.warning() << "The area `" << name << "` can not be added (dupplicate)";
    }
    return nullptr;
}

bool AreaList::loadListFromFile(const fs::path& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
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
    std::string buffer;
    uint line = 0;
    while (std::getline(file, buffer))
    {
        ++line;
        // The area name
        name = buffer;
        name.trim(" \t\n\r");
        if (name.empty())
        {
            continue;
        }

        lname.clear();
        lname = transformNameIntoID(name);
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
    each(
      [&buffer](const Data::Area& area)
      {
          buffer << area.id << '\n';
          auto end = area.links.end();
          for (auto i = area.links.begin(); i != end; ++i)
          {
              buffer << '\t' << (i->second)->with->id << '\n';
          }
      });
}

bool AreaList::saveListToFile(const AnyString& filename) const
{
    if (!filename)
    {
        return false;
    }

    Clob data;
    {
        // Preparing a new list of areas, sorted by their name
        using List = std::list<std::string>;
        List list;
        {
            auto end = areas.end();
            for (auto i = areas.begin(); i != end; ++i)
            {
                list.push_back((i->second)->name.c_str());
            }
            list.sort();
        }
        {
            auto end = list.end();
            for (auto i = list.begin(); i != end; ++i)
            {
                data << *i << '\n';
            }
        }
    }

    // Writing data into the appropriate file
    std::ofstream file(filename);
    if (file.is_open())
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
    each(
      [&ret, &count](const Data::Area& area)
      {
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
    {
        *invalidateCount = count;
    }
    return ret;
}

void AreaList::markAsModified() const
{
    each([](const Data::Area& area) { area.markAsModified(); });
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
    each(
      [&ret, &buffer, &folder, this](const Data::Area& area)
      {
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

static void readAdqPatchMode(Study& study, Area& area)
{
    if (study.header.version < StudyVersion(8, 3))
    {
        return;
    }

    fs::path adqPath = study.folderInput / "areas" / area.id.to<std::string>()
                       / "adequacy_patch.ini";
    IniFile ini;
    if (ini.open(adqPath))
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
                {
                    area.adequacyPatchMode = Data::AdequacyPatch::virtualArea;
                }
                else if (value == "inside")
                {
                    area.adequacyPatchMode = Data::AdequacyPatch::physicalAreaInsideAdqPatch;
                }
                else
                {
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
    // Reset
    area.filterSynthesis = filterAll;
    area.filterYearByYear = filterAll;

    area.nodalOptimization = 0;
    area.spreadUnsuppliedEnergyCost = 0.;
    area.spreadSpilledEnergyCost = 0.;

    bool ret = true;
    const auto studyVersion = study.header.version;

    // DSM, Reserves, D-1
    fs::path reservesPath = (study.folderInput / "reserves" / area.id.to<std::string>())
                              .replace_extension("txt");
    ret = area.reserves.loadFromCSVFile(reservesPath.string(),
                                        fhrMax,
                                        HOURS_PER_YEAR,
                                        Matrix<>::optFixedSize)
          && ret;

    // Optimzation preferences
    if (study.usedByTheSolver)
    {
        if (!study.parameters.include.reserve.dayAhead)
        {
            area.reserves.columnToZero(fhrDayBefore);
        }
        if (!study.parameters.include.reserve.strategic)
        {
            area.reserves.columnToZero(fhrStrategicReserve);
        }
        if (!study.parameters.include.reserve.primary)
        {
            area.reserves.columnToZero(fhrPrimaryReserve);
        }
    }

    // Fatal hors hydro - Misc Gen.
    std::string miscgenName = "miscgen-" + area.id + ".txt";
    fs::path miscgenPath = study.folderInput / "misc-gen" / miscgenName;

    ret = area.miscGen.loadFromCSVFile(miscgenPath.string(),
                                       fhhMax,
                                       HOURS_PER_YEAR,
                                       Matrix<>::optFixedSize)
          && ret;

    // Check misc gen
    {
        buffer.clear() << "Misc Gen: `" << area.id << '`';
        MatrixTestForPositiveValues_LimitWidth(buffer.c_str(), &area.miscGen, fhhPSP);
    }

    // Links
    {
        fs::path folder = study.folderInput / "links" / area.id.c_str();
        ret = AreaLinksLoadFromFolder(study, list, &area, folder) && ret;
    }

    // TODO remove with GUI
    if (JIT::usedFromGUI)
    {
        if (!area.ui)
        {
            area.ui = new AreaUI();
        }

        buffer.clear() << study.folderInput << SEP << "areas" << SEP << area.id << SEP << "ui.ini";
        ret = area.ui->loadFromFile(buffer) && ret;
    }

    bool averageTs = (study.usedByTheSolver && study.parameters.derated);
    // Load
    {
        if (area.load.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            fs::path loadPath = study.folderInput / "load" / "prepro" / area.id.to<std::string>();
            ret = area.load.prepro->loadFromFolder(loadPath) && ret;
        }
        if (!options.loadOnlyNeeded || !area.load.prepro) // Series
        {
            std::string loadId = "load_" + area.id + ".txt";
            fs::path loadSeriesPath = study.folderInput / "load" / "series" / loadId;

            ret = area.load.series.loadFromFile(loadSeriesPath, averageTs) && ret;
        }
    }

    // Solar
    {
        if (area.solar.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            fs::path solarPath = study.folderInput / "solar" / "prepro" / area.id.to<std::string>();
            ret = area.solar.prepro->loadFromFolder(solarPath) && ret;
        }
        if (!options.loadOnlyNeeded || !area.solar.prepro) // Series
        {
            std::string solarId = "solar_" + area.id + ".txt";
            fs::path solarPath = study.folderInput / "solar" / "series" / solarId;
            ret = area.solar.series.loadFromFile(solarPath, averageTs) && ret;
        }
    }

    // Hydro
    {
        // Allocation
        std::string areaIdIni = area.id + ".ini";
        fs::path hydroAlloc = study.folderInput / "hydro" / "allocation" / areaIdIni;
        ret = area.hydro.allocation.loadFromFile(area.id, hydroAlloc) && ret;

        fs::path pathHydro = study.folderInput / "hydro";
        fs::path hydroSeries = pathHydro / "series";

        if (area.hydro.prepro) /* Hydro */
        {
            // if changes are required, please update reloadXCastData()
            fs::path hydroPrepro = pathHydro / "prepro";
            ret = area.hydro.prepro->loadFromFolder(study, area.id, hydroPrepro) && ret;
            ret = area.hydro.prepro->validate(area.id) && ret;
        }

        if (!options.loadOnlyNeeded || !area.hydro.prepro) // Series
        {
            ret = area.hydro.series->loadGenerationTS(area.id, hydroSeries, studyVersion) && ret;
        }

        switch (study.parameters.compatibility.hydroPmax)
        {
        case Parameters::Compatibility::HydroPmax::Daily:
        {
            HydroMaxTimeSeriesReader reader(area.hydro,
                                            area.id.to<std::string>(),
                                            area.name.to<std::string>());
            ret = reader.read(pathHydro.string(), study.usedByTheSolver) && ret;
            break;
        }
        case Parameters::Compatibility::HydroPmax::Hourly:
        {
            ret = area.hydro.series->LoadMaxPower(area.id, hydroSeries) && ret;
            break;
        }
        default:
            throw std::invalid_argument(
              "Value not supported for study.parameters.compatibility.hydroPmax");
        }

        area.hydro.series->resizeTSinDeratedMode(study.parameters.derated,
                                                 studyVersion,
                                                 study.parameters.compatibility.hydroPmax,
                                                 study.usedByTheSolver);
    }

    // Wind
    {
        if (area.wind.prepro) // Prepro
        {
            // if changes are required, please update reloadXCastData()
            fs::path windPath = study.folderInput / "wind" / "prepro" / area.id.to<std::string>();
            ret = area.wind.prepro->loadFromFolder(windPath) && ret;
        }
        if (!options.loadOnlyNeeded || !area.wind.prepro) // Series
        {
            std::string windId = "wind_" + area.id + ".txt";
            fs::path windPath = study.folderInput / "wind" / "series" / windId;
            ret = area.wind.series.loadFromFile(windPath, averageTs) && ret;
        }
    }

    // Thermal cluster list
    {
        fs::path preproPath = study.folderInput / "thermal" / "prepro";
        ret = area.thermal.list.loadPreproFromFolder(study, preproPath) && ret;
        ret = area.thermal.list.validatePrepro(study) && ret;
        fs::path seriesPath = study.folderInput / "thermal" / "series";
        ret = area.thermal.list.loadDataSeriesFromFolder(study, seriesPath) && ret;
        ret = area.thermal.list.loadEconomicCosts(study, seriesPath) && ret;

        // In adequacy mode, all thermal clusters must be in 'mustrun' mode
        if (study.usedByTheSolver && study.parameters.mode == SimulationMode::Adequacy)
        {
            area.thermal.list.enableMustrunForEveryone();
        }
    }

    // Short term storage
    if (studyVersion >= StudyVersion(8, 6))
    {
        fs::path seriesPath = study.folderInput / "st-storage" / "series"
                              / area.id.to<std::string>();

        ret = area.shortTermStorage.loadSeriesFromFolder(seriesPath) && ret;
        ret = area.shortTermStorage.validate() && ret;
    }

    // Renewable cluster list
    if (studyVersion >= StudyVersion(8, 1))
    {
        fs::path seriesPath = study.folderInput / "renewables" / "series";
        ret = area.renewable.list.loadDataSeriesFromFolder(study, seriesPath) && ret;
    }

    // Adequacy patch
    readAdqPatchMode(study, area);

    // Nodal Optimization
    fs::path nodalPath = study.folderInput / "areas" / area.id.to<std::string>()
                         / "optimization.ini";

    IniFile ini;
    if (!ini.open(nodalPath))
    {
        return false;
    }

    ini.each(
      [&area, &nodalPath](const IniFile::Section& section)
      {
          for (auto* p = section.firstProperty; p; p = p->next)
          {
              bool value = p->value.to<bool>();
              CString<30, false> tmp;
              tmp = p->key;
              tmp.toLower();
              if (tmp == "non-dispatchable-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoNonDispatchPower;
                  }
                  continue;
              }
              if (tmp == "dispatchable-hydro-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoDispatchHydroPower;
                  }
                  continue;
              }
              if (tmp == "other-dispatchable-power")
              {
                  if (value)
                  {
                      area.nodalOptimization |= anoOtherDispatchPower;
                  }
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
                      logs.warning() << area.name << ": invalid spread for unsupplied energy cost";
                  }
                  continue;
              }
              if (tmp == "spread-spilled-energy-cost")
              {
                  if (!p->value.to<double>(area.spreadSpilledEnergyCost))
                  {
                      area.spreadSpilledEnergyCost = 0.;
                      logs.warning() << area.name << ": invalid spread for spilled energy cost";
                  }
                  continue;
              }

              logs.warning() << nodalPath << ": Unknown property '" << p->key << "'";
          }
      });

    return ret;
}

void AreaList::ensureDataIsInitialized(Parameters& params, bool loadOnlyNeeded)
{
    AreaListEnsureDataHydroTimeSeries(this);

    if (loadOnlyNeeded)
    {
        // Load
        if (params.isTSGeneratedByPrepro(timeSeriesLoad))
        {
            AreaListEnsureDataLoadPrepro(this);
        }
        // Solar
        if (params.isTSGeneratedByPrepro(timeSeriesSolar))
        {
            AreaListEnsureDataSolarPrepro(this);
        }
        // Hydro
        if (params.isTSGeneratedByPrepro(timeSeriesHydro))
        {
            AreaListEnsureDataHydroPrepro(this);
        }
        // Wind
        if (params.isTSGeneratedByPrepro(timeSeriesWind))
        {
            AreaListEnsureDataWindPrepro(this);
        }
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
    auto studyVersion = pStudy.header.version;

    // Load the list of all available areas
    {
        logs.info() << "Loading the list of areas...";
        fs::path areaListPath = pStudy.folderInput / "areas" / "list.txt";
        ret = loadListFromFile(areaListPath) && ret;
    }

    // Hydro
    {
        logs.info() << "Loading global hydro data...";
        fs::path hydroPath = pStudy.folderInput / "hydro";
        ret = PartHydro::LoadFromFolder(pStudy, hydroPath.string()) && ret;
        ret = PartHydro::validate(pStudy) && ret;
    }

    // Thermal data, specific to areas
    {
        logs.info() << "Loading thermal clusters...";
        fs::path thermalPath = pStudy.folderInput / "thermal";
        fs::path areaIniPath = thermalPath / "areas.ini";
        ret = AreaListLoadThermalDataFromFile(*this, areaIniPath) && ret;

        // The cluster list must be loaded before the method ensureDataIsInitialized is called
        // in order to allocate data with all thermal clusters.
        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            fs::path areaPath = thermalPath / "clusters" / area.id.to<std::string>();
            ret = area.thermal.list.loadFromFolder(pStudy, areaPath, &area) && ret;
            ret = area.thermal.list.validateClusters(pStudy.parameters) && ret;
        }
    }

    // Short term storage data, specific to areas
    if (studyVersion >= StudyVersion(8, 6))
    {
        logs.info() << "Loading short term storage clusters...";
        fs::path stsFolder = pStudy.folderInput / "st-storage";

        if (fs::exists(stsFolder))
        {
            for (const auto& [id, area]: areas)
            {
                fs::path folder = stsFolder / "clusters" / area->id.c_str();

                ret = area->shortTermStorage.createSTStorageClustersFromIniFile(folder) && ret;
                ret = area->shortTermStorage.LoadConstraintsFromIniFile(folder) && ret;
            }
        }
        else
        {
            logs.info() << "Short term storage not found, skipping";
        }
    }

    // Renewable data, specific to areas
    if (studyVersion >= StudyVersion(8, 1))
    {
        // The cluster list must be loaded before the method ensureDataIsInitialized is called
        // in order to allocate data with all renewable clusters.
        fs::path renewClusterPath = pStudy.folderInput / "renewables" / "clusters";

        auto end = areas.end();
        for (auto i = areas.begin(); i != end; ++i)
        {
            Area& area = *(i->second);
            fs::path areaPath = renewClusterPath / area.id.to<std::string>();
            ret = area.renewable.list.loadFromFolder(areaPath, &area) && ret;
            ret = area.renewable.list.validateClusters() && ret;
        }
    }

    // Prepare
    ensureDataIsInitialized(pStudy.parameters, options.loadOnlyNeeded);

    // Load all nodes
    uint indx = 0;
    each(
      [&options, &ret, &buffer, &indx, this](Data::Area& area)
      {
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
    AreaName id = transformNameIntoID(name);
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
            if (lastArea->ui && std::abs(lastArea->ui->x - x) < nearestDistance
                && std::abs(lastArea->ui->y - y) < nearestDistance)
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
    AreaName id = transformNameIntoID(name);
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
            {
                return i->second;
            }
        }
    }
    return nullptr;
}

void AreaListEnsureDataLoadPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Data::Area& area)
      {
          if (!area.load.prepro)
          {
              area.load.prepro = std::make_unique<Antares::Data::Load::Prepro>();
          }
      });
}

void AreaListEnsureDataSolarPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Data::Area& area)
      {
          if (!area.solar.prepro)
          {
              area.solar.prepro = std::make_unique<Antares::Data::Solar::Prepro>();
          }
      });
}

void AreaListEnsureDataWindPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Data::Area& area)
      {
          if (!area.wind.prepro)
          {
              area.wind.prepro = std::make_unique<Antares::Data::Wind::Prepro>();
          }
      });
}

void AreaListEnsureDataHydroTimeSeries(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Data::Area& area)
      {
          if (!area.hydro.series)
          {
              area.hydro.series = std::make_unique<DataSeriesHydro>();
          }
      });
}

void AreaListEnsureDataHydroPrepro(AreaList* l)
{
    /* Asserts */
    assert(l);

    l->each(
      [](Data::Area& area)
      {
          if (!area.hydro.prepro)
          {
              area.hydro.prepro = std::make_unique<PreproHydro>();
          }
      });
}

void AreaListEnsureDataThermalPrepro(AreaList* l)
{
    l->each([](Data::Area& area) { area.thermal.list.ensureDataPrepro(); });
}

uint AreaList::areaLinkCount() const
{
    uint ret = 0;
    each([&ret](const Data::Area& area) { ret += (uint)area.links.size(); });
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
    AreaName newid = transformNameIntoID(newName);
    return renameArea(oldid, newid, newName);
}

bool AreaList::renameArea(const AreaName& oldid, const AreaName& newid, const AreaName& newName)
{
    if (!oldid || !newName || !newid || areas.empty())
    {
        return false;
    }

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
        {
            return false;
        }
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
    each(
      [&oldid](Data::Area& a)
      {
          auto* link = a.findLinkByID(oldid);
          if (!link)
          {
              return;
          }

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
    {
        return;
    }

    list->each(
      [&a](Data::Area& area)
      {
          if (!area.links.empty())
          {
              return;
          }
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
    each([n](Data::Area& area) { area.resizeAllTimeseriesNumbers(n); });
}

void AreaList::fixOrientationForAllInterconnections(
  BindingConstraintsRepository& bindingconstraints)
{
    each(
      [&bindingconstraints](Data::Area& area)
      {
          bool mustLoop;
          // for each link from this area
          do
          {
              // Nothing to do if the area does not have any links
              if (area.links.empty())
              {
                  break;
              }

              // By default, we don't have to loop forever
              mustLoop = false;

              // Foreach link...
              auto end = area.links.end();
              for (auto i = area.links.begin(); i != end; ++i)
              {
                  // Reference to the link
                  auto& link = *(i->second);
                  // Asserts
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
    AreaName lname = transformNameIntoID(id);

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
    {
        return nullptr;
    }
    auto offset = key.find('%');
    if (offset == AreaName::npos || (0 == offset) || (offset == key.size() - 1))
    {
        return nullptr;
    }
    AreaName from(key.c_str(), offset);
    AreaName to(key.c_str() + offset + 1, key.size() - (offset + 1));

    return findLink(from, to);
}

ThermalCluster* AreaList::findClusterFromINIKey(const AnyString& key)
{
    if (key.empty())
    {
        return nullptr;
    }
    auto offset = key.find('.');
    if (offset == AreaName::npos || (0 == offset) || (offset == key.size() - 1))
    {
        return nullptr;
    }
    AreaName parentName(key.c_str(), offset);
    ClusterName id(key.c_str() + offset + 1, key.size() - (offset + 1));
    Area* parentArea = findFromName(parentName);
    if (parentArea == nullptr)
    {
        return nullptr;
    }
    return parentArea->thermal.list.findInAll(id);
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
    each([](Data::Area& area) { area.load.series.reset(); });
}

void AreaList::removeHydroTimeseries()
{
    each([](Data::Area& area) { area.hydro.series->reset(); });
}

void AreaList::removeSolarTimeseries()
{
    each([](Data::Area& area) { area.solar.series.reset(); });
}

void AreaList::removeWindTimeseries()
{
    each([](Data::Area& area) { area.wind.series.reset(); });
}

void AreaList::removeThermalTimeseries()
{
    each(
      [](const Data::Area& area)
      {
          for (const auto& c: area.thermal.list.all())
          {
              c->series.reset();
          }
      });
}

Area::ScratchMap AreaList::buildScratchMap(uint numspace)
{
    Area::ScratchMap scratchmap;
    each([&scratchmap, &numspace](Area& a) { scratchmap.try_emplace(&a, a.scratchpad[numspace]); });
    return scratchmap;
}

} // namespace Antares::Data
