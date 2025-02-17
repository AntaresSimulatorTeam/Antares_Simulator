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

#include "antares/solver/variable/surveyresults.h"
#include "antares/writer/in_memory_writer.h"

using namespace Antares::Solver::Variable;

struct StudyFixture
{
    StudyFixture():
        study(std::make_unique<Antares::Data::Study>())
    {
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->initializeRuntimeInfos();
    }

    std::unique_ptr<Antares::Data::Study> study;
};

BOOST_AUTO_TEST_SUITE(surveyresults)

BOOST_FIXTURE_TEST_CASE(no_variable_constructor_does_not_throw, StudyFixture)
{
    Benchmarking::DurationCollector durationCollector;
    Antares::Solver::InMemoryWriter writer(durationCollector);
    // At least one area was required to trigger a std::bad_alloc throw
    Antares::Data::addAreaToListOfAreas(study->areas, "dummyArea");
    BOOST_CHECK_NO_THROW(SurveyResults survey(*study, "out", writer););
}

BOOST_AUTO_TEST_SUITE_END()
