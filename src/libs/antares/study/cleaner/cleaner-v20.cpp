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

#include <yuni/yuni.h>
#include "../study.h"
#include "../cleaner.h"
#include "../../inifile.h"

using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
namespace // anonymous
{
template<class StringT>
static void PreflightVersion20_area(PathList& e, PathList& p, const Area* area, StringT& buffer)
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
    buffer.clear() << "input/hydro/common/capacity/maxpower_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/common/capacity/reservoir_" << id << ".txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id;
    p.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/ror.txt";
    e.add(buffer);
    buffer.clear() << "input/hydro/series/" << id << "/mod.txt";
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
    buffer.clear() << "input/thermal/series/" << id;
    p.add(buffer);
    buffer.clear() << "input/thermal/prepro/" << id;
    p.add(buffer);
    buffer.clear() << "input/thermal/clusters/" << id;
    p.add(buffer);
    buffer.clear() << "input/thermal/clusters/" << id << "/list.ini";
    e.add(buffer);

    auto end = area->thermal.list.end();
    for (auto i = area->thermal.list.begin(); i != end; ++i)
    {
        // Reference to the thermal cluster
        auto& cluster = *(i->second);

        buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster.id();
        p.add(buffer);
        buffer.clear() << "input/thermal/series/" << id << '/' << cluster.id();
        p.add(buffer);

        buffer.clear() << "input/thermal/series/" << id << '/' << cluster.id() << "/series.txt";
        e.add(buffer);

        buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster.id() << "/data.txt";
        e.add(buffer);
        buffer.clear() << "input/thermal/prepro/" << id << '/' << cluster.id() << "/modulation.txt";
        e.add(buffer);
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

    // Interconnections
    buffer.clear() << "input/links/" << id;
    p.add(buffer);
    buffer.clear() << "input/links/" << id << "/capacities";
    p.add(buffer);
    buffer.clear() << "input/links/" << id << "/properties.ini";
    e.add(buffer);
}

template<class StringT>
static void PreflightVersion20_interco(PathList& p, const Area* area, StringT& buffer)
{
    auto end = area->links.end();
    for (auto i = area->links.begin(); i != end; ++i)
    {
        auto& link = *(i->second);
        // Parameters
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << link.with->id << "_parameters" << ".txt";
        p.add(buffer);

        // Indirect capacities
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << "capacities" << SEP << link.with->id << "_direct" << ".txt";
        p.add(buffer);
        // Direct capacities
        buffer.clear() << "input" << SEP << "links" << SEP << link.from->id << SEP << "capacities" << SEP << link.with->id << "_indirect" << ".txt";
        p.add(buffer);
    }
}

} // anonymous namespace

bool PreflightVersion20(StudyCleaningInfos* infos)
{
    auto* study = new Study();
    study->inputExtension = "txt";
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
    bool ret = infos->customExclude.words(":", [&e](AnyString& word) {
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

                // restoring standard verbosity level
                logs.verbosityLevel = Logs::Verbosity::Debug::level;
            }

            // Exclude
            PreflightVersion20_area(e, p, area, buffer);
            // Clear the memory used by the thermal clusters of the area
            area->thermal.list.clear();

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
            PreflightVersion20_interco(p, area, buffer);
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

        ini.each([&](const IniFile::Section& section) {
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

} // namespace Data
} // namespace Antares
