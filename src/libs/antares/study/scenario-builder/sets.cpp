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
    inUpdaterMode = false;
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

Rules::Ptr Sets::rename(const RulesScenarioName& lname, const RulesScenarioName& newname)
{
    // Checking in a first time if the name already exists
    RulesScenarioName id = newname;
    id.toLower();
    if (id == lname)
    {
        return find(lname);
    }
    if (exists(id))
    {
        return nullptr;
    }

    Rules::Map::iterator i = pMap.find(lname);
    if (i == pMap.end())
    {
        return nullptr;
    }
    Rules::Ptr rules = i->second;
    pMap.erase(i);
    rules->setName(newname);
    pMap[id] = rules;
    return rules;
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

#ifdef BUILD_UI
bool Sets::internalSaveToIniFile(const AnyString& filename) const
{
    // Logs
    {
        logs.info() << "  > Exporting scenario builder data";
        logs.debug() << "[scenario-builder] writing " << filename;
    }

    // Open the file
    IO::File::Stream file;
    if (not file.openRW(filename))
    {
        logs.error() << "Impossible to write " << filename;
        return false;
    }

    // There is no ruleset. Trivial. Aborting.
    if (pMap.empty())
    {
        return true;
    }

    const Rules::Map::const_iterator end = pMap.end();
    for (Rules::Map::const_iterator i = pMap.begin(); i != end; ++i)
    {
        // Alias to the current ruleset
        // Export the informations of the current ruleset
        const Rules::Ptr& ruleset = i->second;
        if (!(!ruleset))
        {
            ruleset->saveToINIFile(file);
        }
    }
    return true;
}
#endif

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
              ruleset.readLine(splitKey, p->value, inUpdaterMode);
          }

          ruleset.sendWarningsForDisabledClusters();
      });
    return true;
}

} // namespace Antares::Data::ScenarioBuilder
