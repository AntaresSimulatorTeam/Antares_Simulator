//
// Created by Nikola Ilic on 23/06/23.
//

#define BOOST_TEST_MODULE hydro - final - level
#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include <antares/study/study.h>
#include "antares/solver/hydro/management/HydroErrorsCollector.h"
#include "antares/solver/hydro/management/finalLevelValidator.h"

#include "include/antares/solver/hydro/management/HydroInputsChecker.h"

using namespace Antares::Solver;
using namespace Antares::Data;

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;

    Fixture()
    {
        // Simulation last day must be 365 so that final level checks succeeds
        study->parameters.simulationDays.end = 365;
        study->parameters.firstMonthInYear = january;
        uint nbYears = study->parameters.nbYears = 2;

        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");

        area_1->hydro.reservoirManagement = true;
        area_2->hydro.reservoirManagement = true;

        area_1->hydro.useWaterValue = false;
        area_2->hydro.useWaterValue = false;

        // Level date must be 0, see hydroAllocationStartMatchesSimulation function
        area_1->hydro.initializeReservoirLevelDate = 0;
        area_2->hydro.initializeReservoirLevelDate = 0;

        area_1->hydro.reservoirCapacity = 340.;
        area_2->hydro.reservoirCapacity = 300.;

        // Set reservoir max and min daily levels, but just for the last day in year
        area_1->hydro.reservoirLevel.resize(3, DAYS_PER_YEAR);
        area_1->hydro.reservoirLevel[PartHydro::minimum][DAYS_PER_YEAR - 1] = 2.4;
        area_1->hydro.reservoirLevel[PartHydro::maximum][DAYS_PER_YEAR - 1] = 6.5;

        area_2->hydro.reservoirLevel.resize(3, DAYS_PER_YEAR);
        area_2->hydro.reservoirLevel[PartHydro::minimum][DAYS_PER_YEAR - 1] = 2.7;
        area_2->hydro.reservoirLevel[PartHydro::maximum][DAYS_PER_YEAR - 1] = 6.4;

        // Resize vector final levels delta with initial levels
        area_1->hydro.deltaBetweenFinalAndInitialLevels.resize(nbYears);
        area_2->hydro.deltaBetweenFinalAndInitialLevels.resize(nbYears);

        // Scenario builder for initial and final reservoir levels
        // -------------------------------------------------------
        uint areasCount = study->areas.size();

        study->parameters.yearsFilter.assign(2, true);

        study->scenarioInitialHydroLevels.resize(nbYears, areasCount);
        study->scenarioFinalHydroLevels.resize(nbYears, areasCount);

        study->scenarioInitialHydroLevels[0][0] = 2.3;
        study->scenarioInitialHydroLevels[0][1] = 4.2;
        study->scenarioInitialHydroLevels[1][0] = 1.5;
        study->scenarioInitialHydroLevels[1][1] = 2.4;

        study->scenarioFinalHydroLevels[0][0] = 3.4;
        study->scenarioFinalHydroLevels[0][1] = 5.1;
        study->scenarioFinalHydroLevels[1][0] = 3.5;
        study->scenarioFinalHydroLevels[1][1] = 4.3;

        // Inflows time series matrices
        // -----------------------------
        uint nbInflowTS = 2;
        // ... Area 1 : Inflows time series numbers for each year
        area_1->hydro.series->timeseriesNumbers.reset(nbYears);
        area_1->hydro.series->timeseriesNumbers[0] = 0;
        area_1->hydro.series->timeseriesNumbers[1] = 1;
        // ... Area 1 : Inflows time series
        area_1->hydro.series->storage.resize(nbInflowTS, 365);
        area_1->hydro.series->storage.timeSeries.fill(200.);
        area_1->hydro.series->storage[0][0] = 200. + 1.;
        area_1->hydro.series->storage[0][DAYS_PER_YEAR - 1] = 200. + 2.;

        // ... Area 2 : time series numbers for each year
        area_2->hydro.series->timeseriesNumbers.reset(nbYears);
        area_2->hydro.series->timeseriesNumbers[0] = 0;
        area_2->hydro.series->timeseriesNumbers[1] = 1;
        // ... Area 2 : Inflows time series
        area_2->hydro.series->storage.resize(nbInflowTS, 365);
        area_2->hydro.series->storage.timeSeries.fill(300.);
        area_2->hydro.series->storage[0][0] = 300. + 1.; // DAYS_PER_YEAR
        area_2->hydro.series->storage[0][DAYS_PER_YEAR - 1] = 300. + 2.;
    }

    ~Fixture() = default;

    Study::Ptr study = std::make_shared<Study>();
    Area* area_1;
    Area* area_2;
    HydroErrorsCollector hydro_errors_collector;
};

BOOST_FIXTURE_TEST_SUITE(final_level_validator, Fixture)

BOOST_AUTO_TEST_CASE(all_parameters_good___check_succeeds_and_final_level_is_usable)
{
    uint year = 0;
    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), true);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), true);
}

BOOST_AUTO_TEST_CASE(no_reservoir_management___check_succeeds_but_final_level_not_usable)
{
    uint year = 0;
    area_1->hydro.reservoirManagement = false;
    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), true);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(use_water_value_is_true___check_succeeds_but_final_level_not_usable)
{
    area_1->hydro.useWaterValue = true;
    uint year = 0;

    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), true);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(final_level_not_set_by_user____check_succeeds_but_final_level_not_usable)
{
    uint year = 0;
    study->scenarioFinalHydroLevels[area_1->index][year] = std::numeric_limits<double>::quiet_NaN();

    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), true);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(
  initial_level_month_and_simulation_first_month_different___check_fails_and_final_level_not_usable)
{
    uint year = 0;
    area_1->hydro.initializeReservoirLevelDate = 3; // initialize reservoir level != January

    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), false);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(simulation_does_last_a_whole_year___check_fails_and_final_level_not_usable)
{
    uint year = 0;
    study->parameters.simulationDays.end = 300;

    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), false);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(final_level_out_of_rule_curves___check_fails_and_final_level_not_usable)
{
    uint year = 0;
    // Rule Curves on last simulation day = [2.4 - 6.5]
    study->scenarioFinalHydroLevels[area_1->index][year] = 6.6;

    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), false);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(
  final_level_unreachable_because_of_too_few_inflows___check_fails_and_final_level_not_usable)
{
    area_1->hydro.reservoirCapacity = 185000;
    uint year = 0;
    study->scenarioInitialHydroLevels[area_1->index][year] = 10;
    study->scenarioFinalHydroLevels[area_1->index][year] = 50;

    // Inflows = 200 MWh/day = 73 000 MWh/year
    // (50 - 10) x Reservoir capacity == 74 000 > 73 000.
    FinalLevelValidator validator(area_1->hydro,
                                  area_1->index,
                                  area_1->name,
                                  study->scenarioInitialHydroLevels[area_1->index][year],
                                  study->scenarioFinalHydroLevels[area_1->index][year],
                                  year,
                                  study->parameters.simulationDays.end,
                                  study->parameters.firstMonthInYear,
                                  hydro_errors_collector);

    BOOST_CHECK_EQUAL(validator.check(), false);
    BOOST_CHECK_EQUAL(validator.finalLevelFineForUse(), false);
}

BOOST_AUTO_TEST_CASE(check_all_areas_final_levels_when_config_is_ok___all_checks_succeed)
{
    HydroInputsChecker hydro_input_checker(*study);

    for (uint year: {0, 1})
    {
        hydro_input_checker.CheckFinalReservoirLevelsConfiguration(year);
    }
    // CheckFinalReservoirLevelsConfiguration(*study, 0);
    // CheckFinalReservoirLevelsConfiguration(*study, 1);

    // Checks on Area 1 modifier
    BOOST_CHECK_EQUAL(area_1->hydro.deltaBetweenFinalAndInitialLevels[0].has_value(), true);
    BOOST_CHECK_EQUAL(area_1->hydro.deltaBetweenFinalAndInitialLevels[1].has_value(), true);
    BOOST_CHECK_EQUAL(area_1->hydro.deltaBetweenFinalAndInitialLevels[0].value(), 3.4 - 2.3);
    BOOST_CHECK_EQUAL(area_1->hydro.deltaBetweenFinalAndInitialLevels[1].value(), 5.1 - 4.2);

    // Checks on Area 2 modifier
    BOOST_CHECK_EQUAL(area_2->hydro.deltaBetweenFinalAndInitialLevels[0].has_value(), true);
    BOOST_CHECK_EQUAL(area_2->hydro.deltaBetweenFinalAndInitialLevels[1].has_value(), true);
    BOOST_CHECK_EQUAL(area_2->hydro.deltaBetweenFinalAndInitialLevels[0].value(), 3.5 - 1.5);
    BOOST_CHECK_EQUAL(area_2->hydro.deltaBetweenFinalAndInitialLevels[1].value(), 4.3 - 2.4);
}

BOOST_AUTO_TEST_SUITE_END()
