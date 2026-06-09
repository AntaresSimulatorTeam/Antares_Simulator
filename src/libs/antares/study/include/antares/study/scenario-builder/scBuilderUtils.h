// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__

#include <string>

#include <yuni/yuni.h>
#include <yuni/core/fwd.h>

namespace Antares::Data::ScenarioBuilder
{
std::string fromHydroLevelToString(double d);
double fromStringToHydroLevel(const Yuni::String& value, const double maxLevel);

uint fromStringToTSnumber(const Yuni::String& value);

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__
