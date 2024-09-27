/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "antares/study/study.h"

#include <yuni/yuni.h>

#include "antares/study/scenario-builder/sets.h"

using namespace Yuni;

namespace Antares::Data
{
// The path to the Icon file to use when writing a study
String StudyIconFile;

void Study::scenarioRulesCreate()
{
    // releasing the previous instance of the scenario builder
    scenarioRules.reset();

    // When ran from the solver, the scenario builder must be present
    scenarioRules = std::make_unique<ScenarioBuilder::Sets>();
    scenarioRules->loadFromStudy(*this);
}

void Study::scenarioRulesDestroy()
{
    scenarioRules.reset();
}

void Study::scenarioRulesLoadIfNotAvailable()
{
    if (!scenarioRules)
    {
        // When ran from the solver, the scenario builder must be present
        scenarioRules = std::make_unique<ScenarioBuilder::Sets>();
        scenarioRules->loadFromStudy(*this);
    }
}

bool Study::modifyAreaNameIfAlreadyTaken(AreaName& out, const AreaName& basename)
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
        {
            return false;
        }

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

} // namespace Antares::Data
