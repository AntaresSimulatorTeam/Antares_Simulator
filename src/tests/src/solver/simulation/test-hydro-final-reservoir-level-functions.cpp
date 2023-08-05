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

void InstantiateAreaHydroStorage(Matrix<double, Yuni::sint32>& storage, double seed, uint nbYears)
{
    for (uint i = 0; i < nbYears; i++)
    {
        for (uint days = 0; days < DAYS_PER_YEAR; days++)
        {
            if (days == 0)
                storage[i][days] = seed + 1;

            if (days == DAYS_PER_YEAR - 1)
                storage[i][days] = seed + 2;

            storage[i][days] = seed;
        }
    }
}

struct Fixture
{
    Fixture(const Fixture& f) = delete;
    Fixture(const Fixture&& f) = delete;
    Fixture& operator=(const Fixture& f) = delete;
    Fixture& operator=(const Fixture&& f) = delete;
    Fixture()
    {
        // Parameters data
        // endDay must be 365, see preCheckStartAndEndSim function
        Parameters& parameters = study->parameters;

        uint& endDay = parameters.simulationDays.end;
        uint& nbYears = parameters.nbYears;
        endDay = 365;
        nbYears = 2;

        // Creating two dummy areas and instantiating necessary values for testing
        area_1 = study->areaAdd("Area1");
        area_2 = study->areaAdd("Area2");

        // Enable reservoir management
        area_1->hydro.reservoirManagement = true;
        area_2->hydro.reservoirManagement = true;

        // Disable water values
        area_1->hydro.useWaterValue = false;
        area_2->hydro.useWaterValue = false;

        // For extracting how many areas are in area map
        AreaList& areas = study->areas;
        Area::Map& areasMap = areas.areas;

        // Level date must be 0, see preCheckStartAndEndSim function
        auto& initializeReservoirLevelDateArea1 = area_1->hydro.initializeReservoirLevelDate;
        auto& initializeReservoirLevelDateArea2 = area_2->hydro.initializeReservoirLevelDate;

        initializeReservoirLevelDateArea1 = 0;
        initializeReservoirLevelDateArea2 = 0;

        // Initialize reservoir capacity
        auto& reservoirCapacityArea1 = area_1->hydro.reservoirCapacity;
        auto& reservoirCapacityArea2 = area_2->hydro.reservoirCapacity;

        reservoirCapacityArea1 = 340.;
        reservoirCapacityArea2 = 300.;

        // Initialize reservoir max and min levels, but just for the last day in year
        auto& reservoirLevelArea1 = area_1->hydro.reservoirLevel;
        auto& reservoirLevelArea2 = area_2->hydro.reservoirLevel;

        reservoirLevelArea1.reset(3, DAYS_PER_YEAR, true);
        reservoirLevelArea2.reset(3, DAYS_PER_YEAR, true);

        auto& reservoirLevelArea1Min
          = reservoirLevelArea1[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
        auto& reservoirLevelArea2Min
          = reservoirLevelArea2[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];

        auto& reservoirLevelArea1Max
          = reservoirLevelArea1[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];
        auto& reservoirLevelArea2Max
          = reservoirLevelArea2[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];

        reservoirLevelArea1Min = 2.4;
        reservoirLevelArea1Max = 6.5;

        reservoirLevelArea2Min = 2.7;
        reservoirLevelArea2Max = 6.4;

        // Defining necessary references for initial and final level matrices
        Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
        Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;

        // Initializing dimension of matrices
        uint width = nbYears;
        uint height = areasMap.size();

        // Setting matrices dimension Scenario Builder Initial and Finals levels matrices
        scenarioInitialHydroLevels.resize(width, height);
        scenarioFinalHydroLevels.resize(width, height);

        // Instantiating matrices with some random values
        scenarioInitialHydroLevels[0][0] = 2.3;
        scenarioInitialHydroLevels[0][1] = 4.2;
        scenarioInitialHydroLevels[1][0] = 1.5;
        scenarioInitialHydroLevels[1][1] = 2.4;

        scenarioFinalHydroLevels[0][0] = 3.4;
        scenarioFinalHydroLevels[0][1] = 5.1;
        scenarioFinalHydroLevels[1][0] = 3.5;
        scenarioFinalHydroLevels[1][1] = 4.3;

        // References for timeseriesNumbers data and instantiating timeseriesNumbers matrices
        Matrix<Yuni::uint32>& timeseriesNumbersArea1 = area_1->hydro.series->timeseriesNumbers;
        Matrix<Yuni::uint32>& timeseriesNumbersArea2 = area_2->hydro.series->timeseriesNumbers;

        timeseriesNumbersArea1.resize(1, nbYears);
        timeseriesNumbersArea2.resize(1, nbYears);

        timeseriesNumbersArea1[0][0] = 0;
        timeseriesNumbersArea1[0][1] = 1;

        timeseriesNumbersArea2[0][0] = 0;
        timeseriesNumbersArea2[0][1] = 1;

        // Instantiating areas storage TS's
        auto& storageArea1 = area_1->hydro.series->storage;
        auto& storageArea2 = area_2->hydro.series->storage;

        storageArea1.resize(nbYears, 365);
        storageArea2.resize(nbYears, 365);

        InstantiateAreaHydroStorage(storageArea1, 200., nbYears);
        InstantiateAreaHydroStorage(storageArea2, 300., nbYears);
    }

    ~Fixture() = default;

    Study::Ptr study = std::make_shared<Study>();
    Area* area_1;
    Area* area_2;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_pass)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);

    // check when all is good
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), true);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_resManagement)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    hydro.reservoirManagement = false;
    year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);

    // check when reservoirManagement = false
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_watValues)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    hydro.useWaterValue = true;
    year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);

    // check when useWaterValue = true
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_initializeData_function_for_area_1_fail_NaN)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    scenarioInitialHydroLevels[1][1] = std::numeric_limits<double>::quiet_NaN();
    auto& hydro = area_1->hydro;
    year = 1;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 1, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);

    // check when finalReservoirLevel = nan
    BOOST_CHECK_EQUAL(finLevInfModify.isActive(), false);
}

BOOST_AUTO_TEST_CASE(Testing_updateInflows_function_for_area_1)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    double expectedEndLevel = 3.4;
    double expectedDeltaLevel = -1.1;
    bool calculatedIncludeFinResLev = finLevInfModify.includeFinalReservoirLevel.at(0);
    double calculatedEndLevel = finLevInfModify.endLevel.at(0);
    double calculatedDeltaLevel = finLevInfModify.deltaLevel.at(0);

    BOOST_CHECK_EQUAL(calculatedIncludeFinResLev, true);
    BOOST_CHECK_EQUAL(calculatedEndLevel, expectedEndLevel);
    BOOST_CHECK_EQUAL(calculatedDeltaLevel, expectedDeltaLevel);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_pass)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(), true);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_preCheckInitResLevel)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;
    // initialize reservoir level != January
    hydro.initializeReservoirLevelDate = 3;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_EndSimDay)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;
    // simulation End Day != 365
    parameters.simulationDays.end = 300;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_RuleCurve)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;
    // Lat Day Rule Curve = [2.4 - 6.5]
    scenarioFinalHydroLevels[0][0] = 6.6;

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(), false);
}

BOOST_AUTO_TEST_CASE(Testing_makeChecks_function_for_area_1_fail_ResCapacity)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;
    auto& hydro = area_1->hydro;
    year = 0;
    hydro.reservoirCapacity = 185000;
    scenarioInitialHydroLevels[0][0] = 10;
    scenarioFinalHydroLevels[0][0] = 50;

    // inflows = 200 MWh/day = 73 000 MWh/year
    // res capacity =  185 000 -> 40% of reservoir is 74 000. So we are missing 1k.

    auto finLevInfModify = FinalLevelInflowsModifier(hydro, 0, "Area1");

    finLevInfModify.initializeData(
      scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, year);
    finLevInfModify.isActive();
    finLevInfModify.updateInflows();

    BOOST_CHECK_EQUAL(finLevInfModify.makeChecks(), false);
}

BOOST_AUTO_TEST_CASE(Testing_prepareFinalReservoirLevelData_function_for_area_1_and_area_2)
{
    auto& hydro1 = area_1->hydro;
    auto& hydro2 = area_2->hydro;
    auto& modifierArea1 = hydro1.finalLevelInflowsModifier;
    auto& modifierArea2 = hydro2.finalLevelInflowsModifier;

    FinalLevelInflowsModifier(hydro1, 0, "Area1");
    FinalLevelInflowsModifier(hydro2, 1, "Area2");

    prepareFinalReservoirLevelData(*study);

    // extract data area 1 - year 1 and 2
    bool area_1_year_1_include_calculated = modifierArea1.includeFinalReservoirLevel.at(0);
    bool area_1_year_2_include_calculated = modifierArea1.includeFinalReservoirLevel.at(1);
    double area_1_year_1_endLev_calculated = modifierArea1.endLevel.at(0);
    double area_1_year_2_endLev_calculated = modifierArea1.endLevel.at(1);
    double area_1_year_1_deltaLev_calculated = modifierArea1.deltaLevel.at(0);
    double area_1_year_2_deltaLev_calculated = modifierArea1.deltaLevel.at(1);
    double area_1_year_1_deltaLev_expected = 2.3 - 3.4;
    double area_1_year_2_deltaLev_expected = 4.2 - 5.1;
    // check data area 1 - year 1 and 2
    BOOST_CHECK_EQUAL(area_1_year_1_include_calculated, true);
    BOOST_CHECK_EQUAL(area_1_year_2_include_calculated, true);
    BOOST_CHECK_EQUAL(area_1_year_1_endLev_calculated, 3.4);
    BOOST_CHECK_EQUAL(area_1_year_2_endLev_calculated, 5.1);
    BOOST_CHECK_EQUAL(area_1_year_1_deltaLev_calculated, area_1_year_1_deltaLev_expected);
    BOOST_CHECK_EQUAL(area_1_year_2_deltaLev_calculated, area_1_year_2_deltaLev_expected);

    // extract data area 2 - year 1 and 2
    bool area_2_year_1_include_calculated = modifierArea2.includeFinalReservoirLevel.at(0);
    bool area_2_year_2_include_calculated = modifierArea2.includeFinalReservoirLevel.at(1);
    double area_2_year_1_endLev_calculated = modifierArea2.endLevel.at(0);
    double area_2_year_2_endLev_calculated = modifierArea2.endLevel.at(1);
    double area_2_year_1_deltaLev_calculated = modifierArea2.deltaLevel.at(0);
    double area_2_year_2_deltaLev_calculated = modifierArea2.deltaLevel.at(1);
    double area_2_year_1_deltaLev_expected = 1.5 - 3.5;
    double area_2_year_2_deltaLev_expected = 2.4 - 4.3;
    // check data area 2 - year 1 and 2
    BOOST_CHECK_EQUAL(area_2_year_1_include_calculated, true);
    BOOST_CHECK_EQUAL(area_2_year_2_include_calculated, true);
    BOOST_CHECK_EQUAL(area_2_year_1_endLev_calculated, 3.5);
    BOOST_CHECK_EQUAL(area_2_year_2_endLev_calculated, 4.3);
    BOOST_CHECK_EQUAL(area_2_year_1_deltaLev_calculated, area_2_year_1_deltaLev_expected);
    BOOST_CHECK_EQUAL(area_2_year_2_deltaLev_calculated, area_2_year_2_deltaLev_expected);
}

BOOST_AUTO_TEST_SUITE_END()