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
#ifndef __ANTARES_SOLVER_TIME_SERIES_GENERATOR_H__
#define __ANTARES_SOLVER_TIME_SERIES_GENERATOR_H__

#include <yuni/yuni.h>
#include <antares/study/fwd.h>
#include <antares/study/parameters.h>
#include <antares/study/parts/thermal/cluster.h>
#include <i_writer.h>
#include "xcast/xcast.h"

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
/*!
** \brief Regenerate the time-series
*/
template<enum Data::TimeSeries T>
bool GenerateTimeSeries(Data::Study& study, uint year);

bool GenerateThermalTimeSeries(Data::Study& study,
                               uint year,
                               bool globalThermalTSgeneration,
                               bool refresh,
                               Antares::Solver::IResultWriter::Ptr writer);

/*!
** \brief Destroy all TS Generators
*/
void DestroyAll(Data::Study& study);

/*!
** \brief Destroy a TS generator if it exists and no longer needed
*/
template<enum Data::TimeSeries T>
void Destroy(Data::Study& study, uint year);

} // namespace TSGenerator
} // namespace Solver
} // namespace Antares

#include "generator.hxx"

#endif // __ANTARES_SOLVER_TIME_SERIES_GENERATOR_H__
