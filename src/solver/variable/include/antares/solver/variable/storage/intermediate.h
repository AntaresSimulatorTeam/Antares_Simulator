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
#ifndef __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__
#define __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include <antares/study/study.h>
#include "antares/antares/constants.h"

#include "../categories.h"
#include "../surveyresults.h"

namespace Antares
{
namespace Solver
{
namespace Variable
{
/*!
** \brief Temporary buffer for allocating results for a single year
**
** This class is mostly used by economic variables for storing
** their data for a single year.
*/
class IntermediateValues final
{
public:
    //! Basic type
    typedef double Type;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    IntermediateValues();
    //! Destructor
    ~IntermediateValues();
    //@}

    void initializeFromStudy(Data::Study& study);

    /*!
    ** \brief Reset all values
    */
    void reset();

    /*!
    ** \brief Compute statistics for the current year
    */
    void computeStatisticsForTheCurrentYear();

    /*!
    ** \brief Compute statistics for the current year
    */
    void computeStatisticsOrForTheCurrentYear();

    /*!
    ** \brief Compute statistics for the current year
    */
    void computeStatisticsAdequacyForTheCurrentYear();

    // Compute averages for the current year from a particular time division results :
    // For instances :
    // - compute averages daily to yearly, from hourly results
    // - compute averages monthly to yearly, from weekly results
    void computeAveragesForCurrentYearFromHourlyResults();
    void computeAveragesForCurrentYearFromDailyResults();
    void computeAveragesForCurrentYearFromWeeklyResults();

    /*!
    ** \brief Compute probabilities for the current year
    */
    void computeProbabilitiesForTheCurrentYear();

    /*!
    ** \brief Make another calculations when values are related to a price
    */
    void adjustValuesWhenRelatedToAPrice();

    /*!
    ** \brief Make another calculations when values are related to a price
    */
    void adjustValuesAdequacyWhenRelatedToAPrice();

    //! \name User reports
    //@{
    template<class VCardT>
    void buildAnnualSurveyReport(SurveyResults& report, int fileLevel, int precision) const;
    //@}

    //! \name Operators
    //@{
    /*!
    ** \brief Vector alias for an hour in the year
    */
    Type& operator[](const uint index);
    const Type& operator[](const uint index) const;
    //@}

    //! Range
    Antares::Data::StudyRangeLimits* pRange;
    //! Calendar, from the study, but dedicated to the output (with leap year)
    Antares::Date::Calendar* calendar;
    //! Range
    Antares::Data::StudyRuntimeInfos* pRuntimeInfo;

    //! Values for each month
    Type month[MONTHS_PER_YEAR];
    //! Values for each week
    Type week[WEEKS_PER_YEAR];
    //! Values for each day in the year
    Type day[DAYS_PER_YEAR];
    //! Values for each hour in the year
    mutable Antares::Memory::Stored<Type>::Type hour;
    //! Year
    Type year;

    template<uint Size, class VCardT, class A>
    void internalExportAnnualValues(SurveyResults& report, const A& array, bool annual) const;

    void computeDailyAveragesForCurrentYear();
    void computeWeeklyAveragesForCurrentYear();
    void computeMonthlyAveragesForCurrentYear();
    void computeYearlyAveragesForCurrentYear();

}; // class IntermediateValues

} // namespace Variable
} // namespace Solver
} // namespace Antares

#include "intermediate.hxx"

#endif // __SOLVER_VARIABLE_STORAGE_INTERMEDIATE_H__
