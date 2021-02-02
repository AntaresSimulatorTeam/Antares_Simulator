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
#include "study.h"
#include "scenario-builder/sets.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
// The path to the Icon file to use when writing a study
String StudyIconFile;

void Study::releaseAllLocks()
{
    // Do nothing for now
}

void Study::scenarioRulesCreate()
{
    // releasing the previous instance of the scenario builder
    delete scenarioRules;

    // When ran from the solver, the scenario builder must be present
    scenarioRules = new ScenarioBuilder::Sets();
    scenarioRules->loadFromStudy(*this);
}

void Study::scenarioRulesCreate(const RulesScenarioName& /*thisoneonly*/)
{
    // releasing the previous instance of the scenario builder
    delete scenarioRules;
    // When ran from the solver, the scenario builder must be present
    scenarioRules = new ScenarioBuilder::Sets();
    scenarioRules->loadFromStudy(*this);
}

void Study::scenarioRulesDestroy()
{
    if (scenarioRules)
    {
        // releasing the previous instance of the scenario builder
        // safety dereferencing for the interface if running
        ScenarioBuilder::Sets* sb = scenarioRules;
        scenarioRules = nullptr;
        delete sb;
    }
}

void Study::scenarioRulesLoadIfNotAvailable()
{
    if (!scenarioRules)
    {
        // When ran from the solver, the scenario builder must be present
        scenarioRules = new ScenarioBuilder::Sets();
        scenarioRules->loadFromStudy(*this);
    }
}

bool Study::areaFindNameForANewArea(AreaName& out, const AreaName& basename)
{
    out = basename;
    AreaName id = out;
    id.toLower();

    if (areas.find(id))
    {
        int i = 1;
        do
        {
            if (++i > 10000)
            {
                out.clear();
                return false;
            }
            out = basename;
            out << "-" << i;
            id = out;
            id.toLower();
        } while (areas.find(id));
    }
    return true;
}

bool Study::TitleFromStudyFolder(const AnyString& folder, String& out, bool warnings)
{
    String b;
    b << folder << IO::Separator << "study.antares";
    StudyHeader header;
    if (header.loadFromFile(b, warnings))
    {
        out = header.caption;
        return true;
    }
    out.clear();
    return false;
}

bool Study::IsRootStudy(const AnyString& folder)
{
    String buffer;
    buffer.reserve(folder.size() + 16);
    return IsRootStudy(folder, buffer);
}

bool Study::IsRootStudy(const AnyString& folder, String& buffer)
{
    buffer.clear() << folder << IO::Separator << "study.antares";
    StudyHeader header;
    return (header.loadFromFile(buffer, false));
}

bool Study::IsInsideStudyFolder(const AnyString& path, String& location, String& title)
{
    if (TitleFromStudyFolder(path, title, false))
    {
        location = path;
        return true;
    }

    String::Size p;
    String::Size offset = 0;
    do
    {
        // Looking for the next folder separator
        p = path.find(IO::Separator, offset);
        if (p >= path.size())
            return false;

        AnyString tmp(path, 0, p);
        if (!tmp.empty())
        {
            if (TitleFromStudyFolder(tmp, title, false))
            {
                location = tmp;
                return true;
            }
        }
        offset = p + 1;
    } while (true);

    return false;
}

} // namespace Data
} // namespace Antares
