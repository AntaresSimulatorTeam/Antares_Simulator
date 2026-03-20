// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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

// TODO remove after vacuum
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
    return (header.loadFromFile(buffer.c_str(), false));
}

} // namespace Antares::Data
