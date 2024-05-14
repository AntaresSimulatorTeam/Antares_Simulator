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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__
#define __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
inline uint Sets::size() const
{
    return (uint)pMap.size();
}

inline bool Sets::empty() const
{
    return pMap.empty();
}

inline Sets::iterator Sets::begin()
{
    return pMap.begin();
}

inline Sets::const_iterator Sets::begin() const
{
    return pMap.begin();
}

inline Sets::iterator Sets::end()
{
    return pMap.end();
}

inline Sets::const_iterator Sets::end() const
{
    return pMap.end();
}

inline bool Sets::exists(const RulesScenarioName& lname) const
{
    return pMap.find(lname) != pMap.end();
}

inline Rules::Ptr Sets::find(const RulesScenarioName& lname) const
{
    using namespace Yuni;
    const_iterator i = pMap.find(lname);
    if (i != pMap.end())
    {
        return i->second;
    }
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
    {
        pMap.clear();
    }
    if (pMap.empty())
    {
        createNew("Default Ruleset");
    }
    return r;
}

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#endif // __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__
