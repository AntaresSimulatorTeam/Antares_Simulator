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
        // endDay must be 365, see preCheckStartAndEndSim function
        Parameters& parameters = study->parameters;

        parameters.simulationDays.end = 365;
        uint nbYears = parameters.nbYears = 2;

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
        Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
        Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;

        uint areasCount = study->areas.size();

        scenarioInitialHydroLevels.resize(nbYears, areasCount);
        scenarioFinalHydroLevels.resize(nbYears, areasCount);

        scenarioInitialHydroLevels[0][0] = 2.3;
        scenarioInitialHydroLevels[0][1] = 4.2;
        scenarioInitialHydroLevels[1][0] = 1.5;
        scenarioInitialHydroLevels[1][1] = 2.4;

        scenarioFinalHydroLevels[0][0] = 3.4;
        scenarioFinalHydroLevels[0][1] = 5.1;
        scenarioFinalHydroLevels[1][0] = 3.5;
        scenarioFinalHydroLevels[1][1] = 4.3;

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

BOOST_AUTO_TEST_CASE(initialize_modifier_for_area_1___modifier_is_active)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, area_1->index, area_1->name);

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), true);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_resManagement)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    hydro.reservoirManagement = false;
    uint year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end, 
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    // check when reservoirManagement = false
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_watValues)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    hydro.useWaterValue = true;
    uint year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    // check when useWaterValue = true
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_NaN)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    scenarioInitialHydroLevels[1][1] = std::numeric_limits<double>::quiet_NaN();
    auto& hydro = area_1->hydro;
    uint year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels, 
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    // check when finalReservoirLevel = -1
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_updateInflows_function_for_area_1)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    finLevInfModify.makeChecks(year);
    finLevInfModify.storeDeltaLevels(year);

    double expectedDeltaLevel = -1.1;
    bool calculatedIncludeFinResLev = finLevInfModify.includeFinalReservoirLevel.at(0);
    double calculatedDeltaLevel = finLevInfModify.deltaLevel.at(0);

    BOOST_CHECK_EQUAL(calculatedIncludeFinResLev, true);
    BOOST_CHECK_EQUAL(calculatedDeltaLevel, expectedDeltaLevel);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_pass)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(year), true);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_preCheckInitResLevel)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;
    // initialize reservoir level != January
    hydro.initializeReservoirLevelDate = 3;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(year), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_EndSimDay)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;
    // simulation End Day != 365
    parameters.simulationDays.end = 300;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(year), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_RuleCurve)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;
    // Lat Day Rule Curve = [2.4 - 6.5]
    scenarioFinalHydroLevels[0][0] = 6.6;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end,
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(year), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_ResCapacity)
{
    auto& parameters = study->parameters;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    uint year = 0;
    hydro.reservoirCapacity = 185000;
    scenarioInitialHydroLevels[0][0] = 10;
    scenarioFinalHydroLevels[0][0] = 50;

    // inflows = 200 MWh/day = 73 000 MWh/year
    // res capacity =  185 000 -> 40% of reservoir is 74 000. So we are missing 1k.

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initialize(scenarioInitialHydroLevels,
                               scenarioFinalHydroLevels,
                               parameters.simulationDays.end, 
                               parameters.nbYears);
    finLevInfModify.ComputeDelta(year);

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(year), false);
}

BOOST_AUTO_TEST_CASE(Testing_prepareFinalReservoirLevelData_function_for_area_1_and_area_2)
{
    auto& hydro1 = area_1->hydro;
    auto& hydro2 = area_2->hydro;
    auto& modifierArea1 = hydro1.finalLevelInflowsModifier;
    auto& modifierArea2 = hydro2.finalLevelInflowsModifier;

    FinalLevelInflowsModifier(hydro1, 0, "Area1");
    FinalLevelInflowsModifier(hydro2, 1, "Area2");

    CheckFinalReservoirLevelsInput(*study);

    // extract data area 1 - year 1 and 2
    bool area_1_year_1_include_calculated = modifierArea1.includeFinalReservoirLevel.at(0);
    bool area_1_year_2_include_calculated = modifierArea1.includeFinalReservoirLevel.at(1);
    double area_1_year_1_deltaLev_calculated = modifierArea1.deltaLevel.at(0);
    double area_1_year_2_deltaLev_calculated = modifierArea1.deltaLevel.at(1);
    double area_1_year_1_deltaLev_expected = 2.3 - 3.4;
    double area_1_year_2_deltaLev_expected = 4.2 - 5.1;
    // check data area 1 - year 1 and 2
    BOOST_CHECK_EQUAL(area_1_year_1_include_calculated, true);
    BOOST_CHECK_EQUAL(area_1_year_2_include_calculated, true);
    BOOST_CHECK_EQUAL(area_1_year_1_deltaLev_calculated, area_1_year_1_deltaLev_expected);
    BOOST_CHECK_EQUAL(area_1_year_2_deltaLev_calculated, area_1_year_2_deltaLev_expected);

    // extract data area 2 - year 1 and 2
    bool area_2_year_1_include_calculated = modifierArea2.includeFinalReservoirLevel.at(0);
    bool area_2_year_2_include_calculated = modifierArea2.includeFinalReservoirLevel.at(1);
    double area_2_year_1_deltaLev_calculated = modifierArea2.deltaLevel.at(0);
    double area_2_year_2_deltaLev_calculated = modifierArea2.deltaLevel.at(1);
    double area_2_year_1_deltaLev_expected = 1.5 - 3.5;
    double area_2_year_2_deltaLev_expected = 2.4 - 4.3;
    // check data area 2 - year 1 and 2
    BOOST_CHECK_EQUAL(area_2_year_1_include_calculated, true);
    BOOST_CHECK_EQUAL(area_2_year_2_include_calculated, true);
    BOOST_CHECK_EQUAL(area_2_year_1_deltaLev_calculated, area_2_year_1_deltaLev_expected);
    BOOST_CHECK_EQUAL(area_2_year_2_deltaLev_calculated, area_2_year_2_deltaLev_expected);
}

BOOST_AUTO_TEST_SUITE_END()