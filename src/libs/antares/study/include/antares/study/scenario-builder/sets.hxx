// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__
#define __LIBS_STUDY_SCENARIO_BUILDER_SETS_HXX__

namespace Antares::Data::ScenarioBuilder
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

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__
