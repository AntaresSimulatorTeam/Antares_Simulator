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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__

#include <string>

#include <yuni/yuni.h>
#include <yuni/core/fwd.h>

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
std::string fromHydroLevelToString(double d);
double fromStringToHydroLevel(const Yuni::String& value, const double maxLevel);

uint fromStringToTSnumber(const Yuni::String& value);

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_UTILS_H__
