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
    DecoratorType::template buildSurveyReport<S, VCardT>(report,
                                                         results,
                                                         dataLevel,
                                                         fileLevel,
                                                         precision);
}

template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
inline void Results<FirstDecoratorT, DecoratorForSpatialAggregateT>::merge(
  uint year,
  const IntermediateValues& data)
{
    DecoratorType::merge(year, data);
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
