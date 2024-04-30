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

#include <yuni/yuni.h>

#include <antares/inifile/inifile.h>
#include "antares/study/cleaner.h"
#include "antares/study/study.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace Antares::Data
{
namespace // anonymous
{
template<class StringT>
static void listOfFilesAnDirectoriesToKeepForArea(PathList& e,
                                                  PathList& p,
                                                  const Area* area,
                                                  StringT& buffer)
{
    // ID of the current area
    const AreaName& id = area->id;

    // Load
    buffer.clear() << "input/load/prepro/" << id;
    p.add(buffer);
    buffer.clear() << "input/load/prepro/" << id << "/data.txt";
    e.add(buffer);
    buffer.clear() << "input/load/prepro/" << id << "/settings.ini";
    e.add(buffer);
    buffer.clear() << "input/load/prepro/" << id << "/k.txt";
    e.add(buffer);
    buffer.clear() << "input/load/prepro/" << id << "/conversion.txt";
    e.add(buffer);
    buffer.clear() << "input/load/prepro/" << id << "/translation.txt";
    e.add(buffer);
    buffer.clear() << "input/load/series/load_" << id << ".txt";
    e.add(buffer);

    // Solar
    buffer.clear() << "input/solar/prepro/" << id;
    p.add(buffer);
    buffer.clear() << "input/solar/prepro/" << id << "/data.txt";
    e.add(buffer);
    buffer.clear() << "input/solar/prepro/" << id << "/settings.ini";
    e.add(buffer);
    buffer.clear() << "input/solar/prepro/" << id << "/k.txt";
    e.add(buffer);
    buffer.clear() << "input/solar/prepro/" << id << "/conversion.txt";
    e.add(buffer);
    buffer.clear() << "input/solar/prepro/" << id << "/translation.txt";
    e.add(buffer);
    buffer.clear() << "input/solar/series/solar_" << id << ".txt";
    e.add(buffer);

    // Hydro
    buffer.clear() << "input/hydro/common/capacity/waterValues_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/creditmodulations_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/inflowPattern_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/maxDailyGenEnergy_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/maxDailyPumpEnergy_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/reservoir_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id;
    p.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/ror.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/mod.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/mingen.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/maxHourlyGenPower.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/maxHourlyPumpPower.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/allocation/" << id << ".ini";
    p.add(buffer);
    buffer.clear() << "input/hydro/prepro/" << id;
    p.add(buffer);
    buffer.clear() << "input/hydro/prepro/" << id << "/prepro.ini";
    e.add(buffer);
    buffer.clear() << "input/hydro/prepro/" << id << "/energy.txt";
    e.add(buffer);

    // Wind
    buffer.clear() << "input/wind/prepro/" << id;
    p.add(buffer);
    buffer.clear() << "input/wind/prepro/" << id << "/data.txt";
    e.add(buffer);
    buffer.clear() << "input/wind/prepro/" << id << "/settings.ini";
    e.add(buffer);
    buffer.clear() << "input/wind/prepro/" << id << "/k.txt";
    e.add(buffer);
    buffer.clear() << "input/wind/prepro/" << id << "/conversion.txt";
    e.add(buffer);
    buffer.clear() << "input/wind/prepro/" << id << "/translation.txt";
    e.add(buffer);
    buffer.clear() << "input/wind/series/wind_" << id << ".txt";
    e.add(buffer);

    // Thermal
    {
        buffer.clear() << "input/thermal/series/" << id;
        p.add(buffer);
        buffer.clear() << "input/thermal/prepro/" << id;
        p.add(buffer);
        buffer.clear() << "input/thermal/clusters/" << id;
        p.add(buffer);
        buffer.clear() << "input/thermal/clusters/" << id << "/list.ini";
        e.add(buffer);

        for (auto& cluster: area->thermal.list.all())
        {
            buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster->id();
            p.add(buffer);
            buffer.clear() << "input/thermal/series/" << id << '/' << cluster->id();
            p.add(buffer);

            buffer.clear() << "input/thermal/series/" << id << '/' << cluster->id()
                           << "/series.txt";
            e.add(buffer);

            buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster->id() << "/data.txt";
            e.add(buffer);
            buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster->id()
                           << "/modulation.txt";
            e.add(buffer);
        }
    }

    // Renewable clusters
    {
        buffer.clear() << "input/renewables/series/" << id;
        p.add(buffer);

        buffer.clear() << "input/renewables/clusters/" << id;
        p.add(buffer);

        buffer.clear() << "input/renewables/clusters/" << id << "/list.ini";
        e.add(buffer);

        for (const auto& cluster: area->renewable.list.all())
        {
            buffer.clear() << "input/renewables/series/" << id << '/' << cluster->id();
            p.add(buffer);

            buffer.clear() << "input/renewables/series/" << id << '/' << cluster->id()
                           << "/series.txt";
            e.add(buffer);
        }
    }

    // Misc-gen
    buffer.clear() << "input/misc-gen/miscgen-" << id << ".txt";
    e.add(buffer);

    // Reserves
    buffer.clear() << "input/reserves/" << id << ".txt";
    e.add(buffer);

    // Areas
    buffer.clear() << "input/areas/" << id;
    p.add(buffer);
    buffer.clear() << "input/areas/" << id << "/ui.ini";
    e.add(buffer);
    buffer.clear() << "input/areas/" << id << "/optimization.ini";
    e.add(buffer);
    buffer.clear() << "input/areas/" << id << "/adequacy_patch.ini";
    e.add(buffer);

    // Interconnections
    buffer.clear() << "input/links/" << id;
    p.add(buffer);
    buffer.clear() << "input/links/" << id << "/capacities";
    p.add(buffer);
    buffer.clear() << "input/links/" << id << "/properties.ini";
    e.add(buffer);
}

template<class StringT>
void listOfFilesAnDirectoriesToKeepForLinks(PathList& p, const Area* area, StringT& buffer)
{
    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);
        // Parameters
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << link.with->id
                       << "_parameters" << ".txt";
        p.add(buffer);

        // Indirect capacities
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << "capacities"
                       << SEP << link.with->id << "_direct" << ".txt";
        p.add(buffer);
        // Direct capacities
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << "capacities"
                       << SEP << link.with->id << "_indirect" << ".txt";
        p.add(buffer);
    }
}

} // anonymous namespace

bool listOfFilesAnDirectoriesToKeep(StudyCleaningInfos* infos)
{
    auto* study = new Study();
    study->relocate(infos->folder);

    PathList& e = infos->exclude;
    PathList& p = infos->postExclude;

    // Getting all files/folders to exclude
    e.add("output");
    e.add("layers");
    e.add("logs");
    e.add("user");
    e.add("Desktop.ini");
    e.add("study.antares");
    e.add("locks/antares.lock");
    e.add("settings/comments.txt");
    e.add("settings/resources/study.ico");
    e.add("settings/generaldata.ini");
    e.add("settings/constraintbuilder.ini");
    e.add("settings/scenariobuilder.dat");
    e.add("input/bindingconstraints/bindingconstraints.ini");
    e.add("input/hydro/hydro.ini");
    e.add("input/areas/list.txt");
    e.add("input/areas/sets.ini");
    e.add("input/links/sets.ini");
    e.add("input/hydro/prepro/correlation.ini");
    e.add("input/wind/prepro/correlation.ini");
    e.add("input/load/prepro/correlation.ini");
    e.add("input/solar/prepro/correlation.ini");
    e.add("input/thermal/areas.ini");

    // Also exclude custom files/folders provided by the user
    infos->customExclude.words(":",
                               [&e](const AnyString& word)
                               {
                                   e.add(word);
                                   return true;
                               });

    // Post
    p.add("logs");
    p.add("output");
    p.add("input");
    p.add("input/links");
    p.add("input/misc-gen");
    p.add("input/reserves");
    p.add("input/areas");
    p.add("input/load");
    p.add("input/load/series");
    p.add("input/load/prepro");
    p.add("input/solar");
    p.add("input/solar/series");
    p.add("input/solar/prepro");
    p.add("input/wind");
    p.add("input/wind/prepro");
    p.add("input/wind/series");
    p.add("input/thermal");
    p.add("input/thermal/prepro");
    p.add("input/thermal/series");
    p.add("input/thermal/clusters");
    p.add("input/renewables");
    p.add("input/renewables/series");
    p.add("input/renewables/clusters");
    p.add("input/hydro");
    p.add("input/hydro/allocation");
    p.add("input/hydro/series");
    p.add("input/hydro/prepro");
    p.add("input/hydro/common");
    p.add("input/hydro/common/capacity");
    p.add("settings");
    p.add("settings/locks");
    p.add("settings/resources");
    p.add("settings/simulations");
    p.add("input/bindingconstraints");

    // Getting all areas
    auto* arealist = new AreaList(*study);
    String buffer;
    buffer.reserve(FILENAME_MAX);
    buffer.clear() << infos->folder << "/input/areas/list.txt";

    // Do not display useless messages
    logs.info() << "  :: analyzing the study data";
    logs.verbosityLevel = Logs::Verbosity::Warning::level;

    if (arealist->loadListFromFile(buffer))
    {
        // restoring standard verbosity level
        logs.verbosityLevel = Logs::Verbosity::Debug::level;

        // Browse all areas
        auto end = arealist->areas.end();
        for (auto i = arealist->areas.begin(); i != end; ++i)
        {
            // Ref to the area
            auto* area = i->second;

            {
                // Do not display useless messages
                logs.verbosityLevel = Logs::Verbosity::Warning::level;
                // Load all thermal clusters
                buffer.clear() << infos->folder << "/input/thermal/clusters/" << area->id;
                if (not area->thermal.list.loadFromFolder(*study, buffer.c_str(), area))
                {
                    delete arealist;
                    delete study;
                    return false;
                }

                buffer.clear() << infos->folder << "/input/renewables/clusters/" << area->id;
                if (not area->renewable.list.loadFromFolder(buffer.c_str(), area))
                {
                    delete arealist;
                    delete study;
                    return false;
                }

                // restoring standard verbosity level
                logs.verbosityLevel = Logs::Verbosity::Debug::level;
            }

            // Exclude
            listOfFilesAnDirectoriesToKeepForArea(e, p, area, buffer);
            // Clear the memory used by the thermal clusters of the area
            area->thermal.list.clearAll();

            // Interconnections
            {
                // Do not display useless messages
                logs.verbosityLevel = Logs::Verbosity::Warning::level;
                // load all links
                buffer.clear() << infos->folder << "/input/links/" << area->id;
                if (not AreaLinksLoadFromFolder(*study, arealist, area, buffer))
                {
                    delete arealist;
                    delete study;
                    return false;
                }
                // restoring standard verbosity level
                logs.verbosityLevel = Logs::Verbosity::Debug::level;
            }
            listOfFilesAnDirectoriesToKeepForLinks(p, area, buffer);
            area->clearAllLinks();
        }
    }
    else
    {
        delete arealist;
        delete study;
        return false;
    }

    // restoring standard verbosity level
    logs.verbosityLevel = Logs::Verbosity::Debug::level;

    delete arealist;

    IniFile ini;
    buffer.clear() << infos->folder << "/input/bindingconstraints/bindingconstraints.ini";
    if (ini.open(buffer))
    {
        String v;

        ini.each(
          [&](const IniFile::Section& section)
          {
              auto* property = section.firstProperty;
              for (; property; property = property->next)
              {
                  if (property->key == "id")
                  {
                      v = property->value;
                      v.toLower();
                      buffer.clear() << "input/bindingconstraints/" << v << ".txt";
                      e.add(buffer);
                      // Go to the next binding constraint
                      break;
                  }
              }
          });
    }
    else
    {
        delete study;
        return false;
    }

    delete study;
    return true;
}

} // namespace Antares::Data
