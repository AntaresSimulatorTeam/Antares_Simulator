/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
#define BOOST_TEST_MODULE "test time series"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/variable/storage/intermediate.h"
#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

#include "dummyVariable.h"

constexpr double TOLERANCE = 1.e-6;
using Antares::Constants::nbHoursInAWeek;

template<unsigned FirstDay, unsigned LastDay>
struct StudyFixture
{
    StudyFixture():
        study(std::make_unique<Antares::Data::Study>())
    {
        study->parameters.simulationDays.first = FirstDay;
        study->parameters.simulationDays.end = LastDay;
        study->parameters.nbYears = 5;
        study->maxNbYearsInParallel = 5;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Antares::Data::Study> study;
};

using FullYearStudyFixture = StudyFixture<0, 365>;
using PartialYearStudyFixture = StudyFixture<0, 192>;

BOOST_AUTO_TEST_SUITE(intermediate_suite)

BOOST_FIXTURE_TEST_CASE(averageFromHourlyFullYear, FullYearStudyFixture)
{
    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10; // hour 0
    intermediate[1] = 20; // hour 1
    intermediate.computeAveragesForCurrentYearFromHourlyResults();

    constexpr int nbHoursInYear = 8736;
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.hour[Antares::Data::rangeCount], nbHoursInYear);

    BOOST_CHECK_CLOSE(intermediate.year, (10. + 20.) / nbHoursInYear, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.week[0], (10. + 20.) / nbHoursInAWeek, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.month[0], (10. + 20.) / (31 * 24), TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.day[0], (10. + 20.) / 24, TOLERANCE);
}

BOOST_FIXTURE_TEST_CASE(averageFromHourlyPartialYear, PartialYearStudyFixture)
{
    Antares::Solver::Variable::IntermediateValues intermediate;
    intermediate.initializeFromStudy(*study);
    intermediate[0] = 10; // hour 0
    intermediate[1] = 20; // hour 1
    intermediate.computeAveragesForCurrentYearFromHourlyResults();

    constexpr int nbWeeks = 27; // std::floor(192 / 7);
    const int nbHoursInYear = nbWeeks * nbHoursInAWeek;
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.week[Antares::Data::rangeCount], nbWeeks);
    BOOST_CHECK_EQUAL(study->runtime.rangeLimits.hour[Antares::Data::rangeCount], nbHoursInYear);

    BOOST_CHECK_CLOSE(intermediate.year, (10. + 20.) / nbHoursInYear, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.week[0], (10. + 20.) / nbHoursInAWeek, TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.month[0], (10. + 20.) / (31 * 24), TOLERANCE);
    BOOST_CHECK_CLOSE(intermediate.day[0], (10. + 20.) / 24, TOLERANCE);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(aggregation)

class ConstantOverScenarios: public Antares::Solver::Variable::DummyVariable<>
{
public:
    double hourlyValue(unsigned int year, unsigned int hour)
    {
        return hour + 1;
    }
};

enum
{
    AVG = 0,
    STD = 1,
    MIN = 2,
    MAX = 3
};

BOOST_FIXTURE_TEST_CASE(averageFromHourlyPartialYear, PartialYearStudyFixture)

{
    using namespace Antares::Solver::Variable;
    study->parameters.resetPlaylist(study->parameters.nbYears);
    Antares::Data::VariablePrintInfo v(Category::FileLevel::va, Category::DataLevel::area);
    v.setMaxColumns(4); // EXP MIN MAX STD
    study->parameters.variablesPrintInfo.add("for_some_reason_this_name_has_no_consequences", v);
    study->parameters.variablesPrintInfo.setAllPrintStatusesTo(true);
    study->parameters.variablesPrintInfo.computeMaxColumnsCountInReports();
    ConstantOverScenarios dm;
    dm.initializeFromStudy(*study);

    std::map<unsigned int, unsigned int> numSpaceToYear{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};

    for (auto [numSpace, year]: numSpaceToYear)
    {
        dm.yearBegin(year, numSpace);
    }
    unsigned int nbYearsForCurrentSummary = study->parameters.nbYears;
    dm.computeSummary(numSpaceToYear, nbYearsForCurrentSummary);

    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);

    SurveyResults survey(*study, "out", writer);
    survey.data.columnIndex = 0;
    dm.buildSurveyReport(survey,
                         Category::DataLevel::area,
                         Category::FileLevel::va,
                         Category::hourly);
    BOOST_CHECK_CLOSE(survey.values[AVG][0], 1, TOLERANCE);
    BOOST_CHECK_CLOSE(survey.values[MIN][0], 1, TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()
