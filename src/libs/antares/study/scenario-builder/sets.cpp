// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/sets.h"

#include <antares/logs/logs.h>
#include "antares/study/study.h"

using namespace Yuni;

namespace Antares::Data::ScenarioBuilder
{
Sets::Sets():
    pStudy(nullptr)
{
}

Sets::~Sets()
{
}

void Sets::setStudy(Study& study)
{
    pStudy = &study;
    assert(pStudy && "Invalid study");
}

void Sets::clear()
{
    assert(pStudy && "Invalid study");
    pMap.clear();
}

bool Sets::loadFromStudy(Study& study)
{
    if (not study.usedByTheSolver)
    {
        logs.info() << "  Loading data for the scenario builder overlay";
    }

    setStudy(study);

    // Loading from the INI file
    std::filesystem::path filename = std::filesystem::path(study.folder.c_str()) / "settings"
                                     / "scenariobuilder.dat";
    bool r = true;
    // If the source code below is changed, please change it in loadFromINIFile too
    if (std::filesystem::exists(filename))
    {
        r = internalLoadFromINIFile(filename.string());
    }
    else
    {
        pMap.clear();
    }

    if (pMap.empty())
    {
        createNew("Default Ruleset");
    }
    return r;
}

Rules::Ptr Sets::createNew(const RulesScenarioName& name)
{
    assert(pStudy != nullptr);

    // Checking in a first time if the name already exists
    RulesScenarioName id = name;
    id.toLower();
    if (exists(id))
    {
        return nullptr;
    }

    // The rule set does not exist, creating a new empty one
    auto newRulesSet = std::make_shared<Rules>(*pStudy);
    newRulesSet->reset();
    newRulesSet->setName(name);
    pMap[id] = newRulesSet;
    return newRulesSet;
}

bool Sets::remove(const RulesScenarioName& lname)
{
    // Checking in a first time if the name already exists
    if (lname.empty())
    {
        return true;
    }

    Rules::Map::iterator i = pMap.find(lname);
    if (i == pMap.end())
    {
        return false;
    }
    pMap.erase(i);
    return true;
}

bool Sets::internalLoadFromINIFile(const AnyString& filename)
{
    // Logs
    logs.info() << "  > loading scenario builder data from " << filename;
    // Cleaning
    pMap.clear();

    IniFile ini;
    if (not ini.open(filename))
    {
        return false;
    }

    ini.each(
      [this](const IniFile::Section& section)
      {
          if (section.name.empty())
          {
              return;
          }

          RulesScenarioName name = section.name;
          name.trim(" \t");
          if (!name)
          {
              return;
          }

          // Create a new ruleset
          Rules::Ptr rulesetptr = createNew(name);
          Rules& ruleset = *rulesetptr;
          AreaName::Vector splitKey;

          for (auto* p = section.firstProperty; p != nullptr; p = p->next)
          {
              p->key.split(splitKey, ",", true, false);
              ruleset.readLine(splitKey, p->value, false);
          }

          ruleset.sendWarningsForDisabledClusters();
      });
    return true;
}

} // namespace Antares::Data::ScenarioBuilder
