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
#ifndef __SOLVER_VARIABLE_STORAGE_RESULTS_H__
#define __SOLVER_VARIABLE_STORAGE_RESULTS_H__

#include <antares/study/study.h>

#include "../categories.h"
#include "fwd.h"
#include "intermediate.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
template<class FirstDecoratorT = Empty, // The first decorator for the results
         template<class, int> class DecoratorForSpatialAggregateT = R::AllYears::Raw>
class Results;

/*!
** \brief
**
** \tparam FirstDecoratorT The first decorator for the class. The common values
**   are `Raw`, `Average`, `Min`, `Max`... This parameter is a static list.
*/
template<class FirstDecoratorT, template<class, int> class DecoratorForSpatialAggregateT>
class Results: public FirstDecoratorT
{
public:
    //! Type of the first decorator
    typedef FirstDecoratorT DecoratorType;

    enum
    {
        //! The count if item in the list
        count = DecoratorType::count,
    };

    enum
    {
        categoryFile = FirstDecoratorT::categoryFile,
    };

public:
    /*!
    ** \brief Initialize result outputs from study
    */
    void initializeFromStudy(Antares::Data::Study&);

    /*!
    ** \brief Reset all values
    */
    void reset();

    /*!
    ** \brief Merge the intermediate values
    */
    void merge(uint year, const IntermediateValues& data);

    template<class S, class VCardT>
    void buildSurveyReport(SurveyResults& report,
                           const S& results,
                           int dataLevel,
                           int fileLevel,
                           int precision) const;

    template<class VCardT>
    void buildDigest(SurveyResults& results, int digestLevel, int dataLevel) const
    {
        // Next
        DecoratorType::template buildDigest<VCardT>(results, digestLevel, dataLevel);
    }

    Antares::Memory::Stored<double>::ConstReturnType hourlyValuesForSpatialAggregate() const
    {
        return DecoratorType::template hourlyValuesForSpatialAggregate<
          DecoratorForSpatialAggregateT>();
    }

}; // class Results

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "average.h"
#include "empty.h"
#include "minmax.h"
#include "raw.h"
#include "results.hxx"
#include "stdDeviation.h"

#endif // __SOLVER_VARIABLE_STORAGE_RESULTS_H__
