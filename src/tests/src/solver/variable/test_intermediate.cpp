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
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Antares::Data::Study> study;
};

BOOST_AUTO_TEST_SUITE(intermediate_suite)
using FullYearStudyFixture = StudyFixture<0, 365>;

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

using PartialYearStudyFixture = StudyFixture<0, 192>;

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
