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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_SOLVER_timeSeries_GENERATOR_H__
#define __ANTARES_SOLVER_timeSeries_GENERATOR_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>
#include <antares/study/parameters.h>
#include <antares/study/study.h>
#include <antares/study/parts/thermal/cluster.h>
#include <antares/writer/i_writer.h>
#include "xcast/xcast.h"

namespace Antares::TSGenerator
{

void ResizeGeneratedTimeSeries(Data::AreaList& areas, Data::Parameters& params);

/*!
** \brief Regenerate the time-series
*/
template<enum Data::TimeSeriesType T>
bool GenerateTimeSeries(Data::Study& study, uint year, IResultWriter& writer);

bool GenerateThermalTimeSeries(Data::Study& study,
                               uint year,
                               bool globalThermalTSgeneration,
                               bool refresh,
                               IResultWriter& writer);

/*!
** \brief Destroy all TS Generators
*/
void DestroyAll(Data::Study& study);

/*!
** \brief Destroy a TS generator if it exists and no longer needed
*/
template<enum Data::TimeSeriesType T>
void Destroy(Data::Study& study, uint year);

} // namespace Antares::TSGenerator

#include "generator.hxx"

#endif // __ANTARES_SOLVER_timeSeries_GENERATOR_H__
