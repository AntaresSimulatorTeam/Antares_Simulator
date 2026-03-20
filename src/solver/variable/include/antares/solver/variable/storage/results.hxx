// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__
#define __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__

namespace Antares::Solver::Variable
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

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_HXX__
