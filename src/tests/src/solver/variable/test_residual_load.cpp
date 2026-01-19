// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test residual load variable"

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include "antares/antares/constants.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/economy/residual.h"

using namespace Antares::Solver::Variable;
using Antares::Constants::nbHoursInAWeek;

namespace
{

// Helper to build a tiny study with one area and required fields
std::unique_ptr<Antares::Data::Study> makeStudy()
{
    auto study = std::make_unique<Antares::Data::Study>();

    study->parameters.simulationDays.first = 0;
    study->parameters.simulationDays.end = 7; // one week
    study->parameters.nbYears = 1;
    study->maxNbYearsInParallel = 1;

    study->initializeRuntimeInfos();

    Antares::Data::addAreaToListOfAreas(study->areas, "area1");

    return study;
}

} // namespace

BOOST_AUTO_TEST_SUITE(residual_load_suite)

BOOST_AUTO_TEST_CASE(hourly_values_are_copied_from_state)
{
    auto study = makeStudy();

    // Build the ResidualLoad variable chain
    Antares::Solver::Variable::Economy::ResidualLoad<> residualLoad;
    residualLoad.initializeFromStudy(*study);

    // Prepare state
    Antares::Solver::Variable::State state(*study);
    state.year = 0;
    state.hourInTheYear = 0;
    state.hourInTheWeek = 0;
    state.numSpace = 0;

    // Attach area
    auto itArea = study->areas.begin();
    BOOST_REQUIRE(itArea != study->areas.end());
    state.area = itArea->second;
    // Create real weekly problem structure with one country
    PROBLEME_HEBDO weeklyProblem;
    weeklyProblem.NombreDePays = 1;
    weeklyProblem.NombreDePasDeTemps = nbHoursInAWeek;
    weeklyProblem.ConsommationsAbattues.resize(nbHoursInAWeek);
    for (unsigned int h = 0; h < nbHoursInAWeek; ++h)
    {
        weeklyProblem.ConsommationsAbattues[h].ConsommationAbattueDuPays.resize(1);
    }

    // Plug weekly problem into state
    state.problemeHebdo = &weeklyProblem;

    // Initialize variable from area (needed to store pArea)
    residualLoad.initializeFromArea(study.get(), state.area);

    residualLoad.simulationBegin();

    // Feed a full week of values
    const double baseValue = 10.0;
    for (unsigned int h = 0; h < nbHoursInAWeek; ++h)
    {
        state.hourInTheYear = h;
        state.hourInTheWeek = h;
        weeklyProblem.ConsommationsAbattues[h].ConsommationAbattueDuPays[0] = baseValue + h;

        residualLoad.hourForEachArea(state, state.numSpace);
    }

    // For this test we just check that raw hourly values were stored correctly
    auto raw = residualLoad.retrieveRawHourlyValuesForCurrentYear(0, state.numSpace);
    for (unsigned int h = 0; h < nbHoursInAWeek; ++h)
    {
        BOOST_CHECK_EQUAL(raw[h], baseValue + h);
    }
}

BOOST_AUTO_TEST_SUITE_END()
