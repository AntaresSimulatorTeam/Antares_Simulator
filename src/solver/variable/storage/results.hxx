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
#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__
#define __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
inline void Results<FirstDecoratorT, DecoratorForSpatialAggregateT>::initializeFromStudy(
  Antares::Data::Study& study)
{
    DecoratorType::initializeFromStudy(study);
}

template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
template<class S, class VCardT>
inline void Results<FirstDecoratorT, DecoratorForSpatialAggregateT>::buildSurveyReport(
  SurveyResults& report,
  const S& results,
  int dataLevel,
  int fileLevel,
  int precision) const
{
    // Ask to fullfil the report to the first decorator
    DecoratorType::template buildSurveyReport<S, VCardT>(
      report, results, dataLevel, fileLevel, precision);
}

template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
inline void Results<FirstDecoratorT, DecoratorForSpatialAggregateT>::merge(
  uint year,
  const IntermediateValues& data)
{
    DecoratorType::merge(year, data);
    data.flush();
}

template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
inline void Results<FirstDecoratorT, DecoratorForSpatialAggregateT>::reset()
{
    DecoratorType::reset();
}

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__
