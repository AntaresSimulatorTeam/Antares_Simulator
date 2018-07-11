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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__
# define __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__


namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{


	inline uint Sets::size() const
	{
		return (uint) pMap.size();
	}


	inline bool  Sets::empty() const
	{
		return pMap.empty();
	}



	inline Sets::iterator  Sets::begin()
	{
		return pMap.begin();
	}


	inline Sets::const_iterator  Sets::begin() const
	{
		return pMap.begin();
	}


	inline Sets::iterator  Sets::end()
	{
		return pMap.end();
	}


	inline Sets::const_iterator  Sets::end() const
	{
		return pMap.end();
	}


	inline bool Sets::exists(const RulesScenarioName& lname) const
	{
		return pMap.find(lname) != pMap.end();
	}


	inline Rules::Ptr  Sets::find(const RulesScenarioName& lname) const
	{
		using namespace Yuni;
		const_iterator i = pMap.find(lname);
		if (i != pMap.end())
			return i->second;
		return nullptr;
	}


	template<class StringT>
	inline bool Sets::saveToINIFile(const StringT& filename)
	{
		const AnyString adapter(filename);
		return internalSaveToIniFile(adapter);
	}


	template<class StringT>
	bool Sets::loadFromINIFile(const StringT& filename)
	{
		// If the source code below is changed, please change it in loadFromStudy too
		const AnyString adapter(filename);
		bool r = internalLoadFromINIFile(adapter);
		if (!r)
			pMap.clear();
		if (pMap.empty())
			createNew("Default Ruleset");
		return r;
	}




} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#endif // __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__
