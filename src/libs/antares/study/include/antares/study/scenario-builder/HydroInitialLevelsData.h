// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_INITIAL_LEVELS_DATA_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_INITIAL_LEVELS_DATA_H__

#include "HydroLevelsData.h"

namespace Antares::Data::ScenarioBuilder
{
/*!
** \brief Rules for initial hydro levels, for all years and areas
*/
class HydroInitialLevelsData final: public HydroLevelsData
{
public:
    bool apply(Study& study) override;
}; // class HydroInitialLevelsData

} // namespace Antares::Data::ScenarioBuilder

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_INITIAL_LEVELS_DATA_H__
