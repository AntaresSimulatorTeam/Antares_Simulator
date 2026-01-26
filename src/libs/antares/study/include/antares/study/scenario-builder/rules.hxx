// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_RULES_HXX__
#define __LIBS_STUDY_SCENARIO_BUILDER_RULES_HXX__

namespace Antares::Data::ScenarioBuilder
{
inline uint Rules::areaCount() const
{
    return pAreaCount;
}

inline const RulesScenarioName& Rules::name() const
{
    return pName;
}

inline void Rules::setName(RulesScenarioName name)
{
    pName = name;
}

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
