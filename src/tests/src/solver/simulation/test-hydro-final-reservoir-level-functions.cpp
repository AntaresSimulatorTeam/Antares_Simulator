//
// Created by Nikola Ilic on 23/06/23.
//

#define BOOST_TEST_MODULE hydro - final - level
#define WIN32_LEAN_AND_MEAN
#include <boost/test/unit_test.hpp>

#include "hydro-final-reservoir-level-functions.h"
#include <study.h>

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
        uint nbYears = study->parameters.nbYears = 2;

        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");

        area_1->hydro.reservoirManagement = true;
        area_2->hydro.reservoirManagement = true;

        area_1->hydro.useWaterValue = false;
        area_2->hydro.useWaterValue = false;

        // Level date must be 0, see preCheckStartAndEndSim function
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
        area_1->hydro.series->timeseriesNumbers.resize(1, nbInflowTS);
        area_1->hydro.series->timeseriesNumbers[0][0] = 0;
        area_1->hydro.series->timeseriesNumbers[0][1] = 1;
        // ... Area 1 : Inflows time series
        area_1->hydro.series->storage.resize(nbInflowTS, 365);
        area_1->hydro.series->storage.fill(200.);
        area_1->hydro.series->storage[0][0] = 200. + 1.;
        area_1->hydro.series->storage[0][DAYS_PER_YEAR - 1] = 200. + 2.;

        // ... Area 2 : time series numbers for each year
        area_2->hydro.series->timeseriesNumbers.resize(1, nbInflowTS);
        area_2->hydro.series->timeseriesNumbers[0][0] = 0;
        area_2->hydro.series->timeseriesNumbers[0][1] = 1;
        // ... Area 2 : Inflows time series
        area_2->hydro.series->storage.resize(nbInflowTS, 365);
        area_2->hydro.series->storage.fill(300.);
        area_2->hydro.series->storage[0][0] = 300. + 1.; //DAYS_PER_YEAR
        area_2->hydro.series->storage[0][DAYS_PER_YEAR - 1] = 300. + 2.;
    }

    ~Fixture() = default;

    Study::Ptr study = std::make_shared<Study>();
    Area* area_1;
    Area* area_2;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(initialize_modifier_for_area_1___modifier_is_applicable)
{
    uint year = 0;
    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    finLevInfModify.CheckInfeasibility(year);

    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), true);
}

BOOST_AUTO_TEST_CASE(reservoir_management_is_false_for_area_1___modifier_is_not_applicable)
{
    area_1->hydro.reservoirManagement = false;
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    finLevInfModify.CheckInfeasibility(year);

    // check when reservoirManagement = false
    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), false);
}

BOOST_AUTO_TEST_CASE(use_water_value_is_true_for_area_1___modifier_is_not_applicable)
{
    area_1->hydro.useWaterValue = true;
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    finLevInfModify.CheckInfeasibility(year);

    // check when useWaterValue = true
    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), false);
}

BOOST_AUTO_TEST_CASE(initial_level_from_scenariobuilder_is_NaN_for_area_1_and_year_0____modifier_not_applicable)
{
    uint year = 0;
    study->scenarioInitialHydroLevels[area_1->index][year] = std::numeric_limits<double>::quiet_NaN();

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    finLevInfModify.CheckInfeasibility(year);

    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), false);
}

BOOST_AUTO_TEST_CASE(final_level_modifier_not_initialized____modifier_not_applicable)
{
    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);
    uint year = 0;
    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), false);
}

BOOST_AUTO_TEST_CASE(checking_level_configuration_is_ok_for_area_1_and_year_0___delta_level_as_expected)
{
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    finLevInfModify.CheckInfeasibility(year);

    double expectedDeltaLevel = -1.1;
    BOOST_CHECK_EQUAL(finLevInfModify.isApplicable(year), true);
    BOOST_CHECK_EQUAL(finLevInfModify.deltaLevel.at(year), expectedDeltaLevel);
}

BOOST_AUTO_TEST_CASE(input_level_configuration_has_nothing_wrong_for_area_1___check_succeeds)
{
    uint year = 0;
    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    BOOST_CHECK_EQUAL(finLevInfModify.CheckInfeasibility(year), true);
}

BOOST_AUTO_TEST_CASE(initial_level_month_for_area_1_is_not_january___check_fails)
{
    uint year = 0;
    area_1->hydro.initializeReservoirLevelDate = 3; // initialize reservoir level != January

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    BOOST_CHECK_EQUAL(finLevInfModify.CheckInfeasibility(year), false);
}

BOOST_AUTO_TEST_CASE(simulation_last_day_is_not_365___check_fails)
{
    uint year = 0;
    study->parameters.simulationDays.end = 300;

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    BOOST_CHECK_EQUAL(finLevInfModify.CheckInfeasibility(year), false);
}

BOOST_AUTO_TEST_CASE(Final_level_not_between_rule_curves_for_area_1___check_fails)
{
    uint year = 0;
    // Rule Curves on last simulation day = [2.4 - 6.5]
    study->scenarioFinalHydroLevels[area_1->index][year] = 6.6;

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    BOOST_CHECK_EQUAL(finLevInfModify.CheckInfeasibility(year), false);
}

BOOST_AUTO_TEST_CASE(diff_between_init_and_final_levels_are_bigger_than_yearly_inflows_for_area_1___check_fails)
{
    area_1->hydro.reservoirCapacity = 185000;
    uint year = 0;
    study->scenarioInitialHydroLevels[area_1->index][year] = 10;
    study->scenarioFinalHydroLevels[area_1->index][year] = 50;

    // Inflows = 200 MWh/day = 73 000 MWh/year
    // (50 - 10) x Reservoir capacity == 74 000 > 73 000.

    auto finLevInfModify = FinalLevelInflowsModifier(area_1->hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(study->scenarioInitialHydroLevels,
                               study->scenarioFinalHydroLevels,
                               study->parameters.simulationDays.end,
                               study->parameters.nbYears);

    BOOST_CHECK_EQUAL(finLevInfModify.CheckInfeasibility(year), false);
}

BOOST_AUTO_TEST_CASE(check_all_areas_final_reservoir_levels_when_config_is_ok___all_checks_succeed)
{
    CheckFinalReservoirLevelsConfiguration(*study);

    // Checks on Area 1 modifier
    BOOST_CHECK_EQUAL(area_1->hydro.finalLevelInflowsModifier.isApplicable(0), true);
    BOOST_CHECK_EQUAL(area_1->hydro.finalLevelInflowsModifier.isApplicable(1), true);
    BOOST_CHECK_EQUAL(area_1->hydro.finalLevelInflowsModifier.deltaLevel.at(0), 2.3 - 3.4);
    BOOST_CHECK_EQUAL(area_1->hydro.finalLevelInflowsModifier.deltaLevel.at(1), 4.2 - 5.1);

    // Checks on Area 2 modifier
    BOOST_CHECK_EQUAL(area_2->hydro.finalLevelInflowsModifier.isApplicable(0), true);
    BOOST_CHECK_EQUAL(area_2->hydro.finalLevelInflowsModifier.isApplicable(1), true);
    BOOST_CHECK_EQUAL(area_2->hydro.finalLevelInflowsModifier.deltaLevel.at(0), 1.5 - 3.5);
    BOOST_CHECK_EQUAL(area_2->hydro.finalLevelInflowsModifier.deltaLevel.at(1), 2.4 - 4.3);
}

BOOST_AUTO_TEST_SUITE_END()