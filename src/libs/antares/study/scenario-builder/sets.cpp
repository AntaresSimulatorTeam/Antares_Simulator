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

#include "sets.h"
#include "../study.h"
#include "../../logs.h"

using namespace Yuni;

#define SEP  IO::Separator



namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{

	Sets::Sets() :
		pStudy(nullptr)
	{
		inUpdaterMode=false;
	}


	Sets::~Sets()
	{
	}


	void Sets::clear()
	{
		assert(pStudy && "Invalid study");
		pMap.clear();
	}


	bool Sets::loadFromStudy(const Study& study)
	{
		if (not study.usedByTheSolver)
			logs.info() << "  Loading data for the scenario builder overlay";

		// Reset / clear
		pStudy = &study;
		assert(pStudy && "Invalid study");

		// Loading from the INI file
		String filename;
		filename << study.folder << SEP << "settings" << SEP << "scenariobuilder.dat";
		bool r = true;
		// If the source code below is changed, please change it in loadFromINIFile too
		if (study.header.version >= 360 && IO::Exists(filename))
			r = internalLoadFromINIFile(filename);
		else
			pMap.clear();

		if (pMap.empty())
			createNew("Default Ruleset");
		return r;
	}


	Rules::Ptr Sets::createNew(const RulesScenarioName& name)
	{
		assert(pStudy != nullptr);

		// Checking in a first time if the name already exists
		RulesScenarioName id = name;
		id.toLower();
		if (exists(id))
			return nullptr;

		// The rule set does not exist, creating a new empty one
		Rules::Ptr newRulesSet = new Rules();
		newRulesSet->reset(*pStudy);
		newRulesSet->pName = name;
		pMap[id] = newRulesSet;
		return newRulesSet;
	}


	Rules::Ptr  Sets::rename(const RulesScenarioName& lname, const RulesScenarioName& newname)
	{
		// Checking in a first time if the name already exists
		RulesScenarioName id = newname;
		id.toLower();
		if (id == lname)
			return find(lname);
		if (exists(id))
			return nullptr;

		Rules::Map::iterator i = pMap.find(lname);
		if (i == pMap.end())
			return nullptr;
		Rules::Ptr rules = i->second;
		pMap.erase(i);
		rules->pName = newname;
		pMap[id] = rules;
		return rules;
	}


	bool  Sets::remove(const RulesScenarioName& lname)
	{
		// Checking in a first time if the name already exists
		if (lname.empty())
			return true;

		Rules::Map::iterator i = pMap.find(lname);
		if (i == pMap.end())
			return false;
		pMap.erase(i);
		return true;
	}


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
			return true;

		const Rules::Map::const_iterator end = pMap.end();
		for (Rules::Map::const_iterator i = pMap.begin(); i != end; ++i)
		{
			// Alias to the current ruleset
			// Export the informations of the current ruleset
			const Rules::Ptr& ruleset = i->second;
			if (!(!ruleset))
				ruleset->saveToINIFile(*pStudy, file);
		}
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
			return false;

		ini.each([&] (const IniFile::Section& section)
		{
			if (section.name.empty())
				return;

			RulesScenarioName name = section.name;
			name.trim(" \t");
			if (!name)
				return;

			// Create a new ruleset
			Rules::Ptr  rulesetptr = createNew(name);
			Rules& ruleset = *rulesetptr;
			AreaName::Vector instrs;

			for (auto* p = section.firstProperty; p != nullptr; p = p->next)
			{
				uint value = p->value.to<uint>();
				if (value)
				{
					p->key.split(instrs, ",", true, false);
					if (instrs.size() > 2)
						ruleset.loadFromInstrs(*pStudy, instrs, value, inUpdaterMode);
				}
			}
		});
		return true;
	}





} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

