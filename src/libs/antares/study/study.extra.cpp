// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/study.h"

#include <boost/algorithm/string/case_conv.hpp>

#include "antares/study/scenario-builder/sets.h"

namespace Antares::Data
{

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
