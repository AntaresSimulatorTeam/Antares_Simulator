// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_H__
#define __SOLVER_VARIABLE_STORAGE_RESULTS_H__

#include <tuple>
#include <type_traits>
#include <utility>

#include <antares/study/study.h>

#include "../categories.h"
#include "average.h"
#include "empty.h"
#include "fwd.h"
#include "intermediate.h"
#include "minmax.h"
#include "raw.h"
#include "stdDeviation.h"

namespace Antares::Solver::Variable
{

namespace detail
{
template<class T, class Tuple>
struct TupleContains;

template<class T>
struct TupleContains<T, std::tuple<>>: std::false_type
{
};

template<class T, class Head, class... Tail>
struct TupleContains<T, std::tuple<Head, Tail...>>
    : std::conditional_t<std::is_same_v<T, Head>,
                         std::true_type,
                         TupleContains<T, std::tuple<Tail...>>>
{
};

template<class T, class Tuple>
inline constexpr bool tuple_can_contain_type_v = TupleContains<T, Tuple>::value;

} // namespace detail

template<class DecoratorTupleT = std::tuple<>, class SpatialAggT = R::AllYears::Raw>
class Results;

using StandardAllYearsDecorators = std::
  tuple<R::AllYears::Average, R::AllYears::StdDeviation, R::AllYears::Min, R::AllYears::Max>;

template<class SpatialAggT = R::AllYears::Raw>
using StandardResults = Results<StandardAllYearsDecorators, SpatialAggT>;

template<class DecoratorTupleT, class SpatialAggT>
class Results
{
public:
    using DecoratorTuple = DecoratorTupleT;

    static constexpr std::size_t count = std::tuple_size_v<DecoratorTuple>;

private:
    template<class Tup>
    struct CategoryFileFold;

    template<class... Decorators>
    struct CategoryFileFold<std::tuple<Decorators...>>
    {
        static constexpr int value = (0 | ... | Decorators::categoryFile);
    };

public:
    static constexpr int categoryFile = CategoryFileFold<DecoratorTuple>::value;

    void initializeFromStudy(Antares::Data::Study& study)
    {
        std::apply([&](auto&... d) { (d.initializeFromStudy(study), ...); }, decorators_);
    }

    void reset()
    {
        std::apply([](auto&... d) { (d.reset(), ...); }, decorators_);
    }

    void merge(uint year, const IntermediateValues& data)
    {
        std::apply([&](auto&... d) { (d.merge(year, data), ...); }, decorators_);
    }

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const
    {
        std::apply(
          [&](const auto&... d)
          {
              (d.template buildSurveyReport<S, VCardT>(report,
                                                       results,
                                                       dataLevel,
                                                       fileLevel,
                                                       precision),
               ...);
          },
          decorators_);
    }

    template<class VCardT>
    void buildDigest(SurveyResults& report, int digestLevel, int dataLevel) const
    {
        std::apply([&](const auto&... d)
                   { (d.template buildDigest<VCardT>(report, digestLevel, dataLevel), ...); },
                   decorators_);
    }

    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        if constexpr (detail::tuple_can_contain_type_v<SpatialAggT, DecoratorTuple>)
        {
            const auto& d = std::get<SpatialAggT>(decorators_);
            if constexpr (requires { d.hourlyForSpatialAggregate(); })
            {
                return d.hourlyForSpatialAggregate();
            }
            else
            {
                return Antares::Memory::Stored<double>::NullValue();
            }
        }
        else
        {
            return Antares::Memory::Stored<double>::NullValue();
        }
    }

    auto& avgdata()
    {
        static_assert(detail::tuple_can_contain_type_v<R::AllYears::Average, DecoratorTuple>,
                      "avgdata() requires an R::AllYears::Average decorator");
        return std::get<R::AllYears::Average>(decorators_).avgdata;
    }

    const auto& avgdata() const
    {
        static_assert(detail::tuple_can_contain_type_v<R::AllYears::Average, DecoratorTuple>,
                      "avgdata() requires an R::AllYears::Average decorator");
        return std::get<R::AllYears::Average>(decorators_).avgdata;
    }

    auto& rawdata()
    {
        static_assert(detail::tuple_can_contain_type_v<R::AllYears::Raw, DecoratorTuple>,
                      "rawdata() requires an R::AllYears::Raw decorator");
        return std::get<R::AllYears::Raw>(decorators_).rawdata;
    }

    const auto& rawdata() const
    {
        static_assert(detail::tuple_can_contain_type_v<R::AllYears::Raw, DecoratorTuple>,
                      "rawdata() requires an R::AllYears::Raw decorator");
        return std::get<R::AllYears::Raw>(decorators_).rawdata;
    }

private:
    DecoratorTuple decorators_;
}; // class Results

} // namespace Antares::Solver::Variable

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_H__
