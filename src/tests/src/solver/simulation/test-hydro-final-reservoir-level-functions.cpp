//
// Created by Nikola Ilic on 23/06/23.
//
/*
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

BOOST_AUTO_TEST_CASE(Testing_initializeGeneralData_function_for_area_1)
{
    auto& parameters = study->parameters;
    auto& year = parameters.nbYears;
    year = 5;

    auto& RuntimeData = area_1->hydro.finalReservoirLevelRuntimeData;

    initializeGeneralData(RuntimeData, parameters, year);

    BOOST_CHECK_EQUAL(RuntimeData.simEndDay, parameters.simulationDays.end);
    BOOST_CHECK_EQUAL(RuntimeData.yearIndex, year);
}

BOOST_AUTO_TEST_CASE(Testing_initializePerAreaData_function_for_area_2)
{
    FinalReservoirLevelRuntimeData& finLevData = area_2->hydro.finalReservoirLevelRuntimeData;
    auto& initialReservoirLevel = finLevData.initialReservoirLevel;
    auto& finalReservoirLevel = finLevData.finalReservoirLevel;
    auto& deltaReservoirLevel = finLevData.deltaReservoirLevel;

    initialReservoirLevel = 0.;
    finalReservoirLevel = 0.;
    deltaReservoirLevel = 0.;
    finLevData.yearIndex = 0;

    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;

    auto& InitialHydroLevelsRef = scenarioInitialHydroLevels[1][0];
    auto& FinalHydroLevelsRef = scenarioFinalHydroLevels[1][0];

    initializePerAreaData(
      finLevData, scenarioInitialHydroLevels, scenarioFinalHydroLevels, *area_2);

    BOOST_CHECK_EQUAL(initialReservoirLevel, InitialHydroLevelsRef);
    BOOST_CHECK_EQUAL(finalReservoirLevel, FinalHydroLevelsRef);
    BOOST_CHECK_EQUAL(deltaReservoirLevel, (InitialHydroLevelsRef - FinalHydroLevelsRef));
}

BOOST_AUTO_TEST_CASE(Testing_initializePreCheckData_function_for_area_2)
{
    FinalReservoirLevelRuntimeData& finLevData = area_2->hydro.finalReservoirLevelRuntimeData;

    auto& initReservoirLvlMonth = finLevData.initReservoirLvlMonth;
    auto& reservoirCapacity = finLevData.reservoirCapacity;

    auto& HydroinitializeReservoirLevelDate = area_2->hydro.initializeReservoirLevelDate;
    auto& HydroreservoirCapacity = area_2->hydro.reservoirCapacity;

    HydroinitializeReservoirLevelDate = 0;
    HydroreservoirCapacity = 250.3;

    initializePreCheckData(finLevData, *area_2);

    BOOST_CHECK_EQUAL(initReservoirLvlMonth, HydroinitializeReservoirLevelDate);
    BOOST_CHECK_EQUAL(reservoirCapacity, HydroreservoirCapacity);
}

BOOST_AUTO_TEST_CASE(Testing_ruleCurveForSimEndReal_function_for_area_1)
{
    FinalReservoirLevelRuntimeData& finLevData = area_1->hydro.finalReservoirLevelRuntimeData;
    auto& reservoirLevel = area_1->hydro.reservoirLevel;

    auto& lowLevelLastDay = finLevData.lowLevelLastDay;
    auto& highLevelLastDay = finLevData.highLevelLastDay;

    auto& HydroreservoirLevelMinimum = reservoirLevel[Data::PartHydro::minimum][DAYS_PER_YEAR - 1];
    auto& HydroreservoirLevelMaximum = reservoirLevel[Data::PartHydro::maximum][DAYS_PER_YEAR - 1];

    HydroreservoirLevelMinimum = 5.7;
    HydroreservoirLevelMaximum = 13.5;

    ruleCurveForSimEndReal(finLevData, *area_1);

    BOOST_CHECK_EQUAL(HydroreservoirLevelMinimum, lowLevelLastDay);
    BOOST_CHECK_EQUAL(HydroreservoirLevelMaximum, highLevelLastDay);
}

BOOST_AUTO_TEST_CASE(calculateTotalInflows_function_for_area_1)
{
    FinalReservoirLevelRuntimeData& finLevData = area_1->hydro.finalReservoirLevelRuntimeData;

    auto& inflowsmatrix = area_1->hydro.series->storage;
    auto& srcinflows = inflowsmatrix[0];

    double total_inflow_expected = DAYS_PER_YEAR * 200;
    double total_inflow_calculated = finLevData.calculateTotalInflows(srcinflows);

    BOOST_CHECK_EQUAL(total_inflow_expected, total_inflow_calculated);
}

BOOST_AUTO_TEST_CASE(preCheckStartAndEndSim_function_for_area_1)
{
    FinalReservoirLevelRuntimeData& finLevData = area_1->hydro.finalReservoirLevelRuntimeData;

    const AreaName& areaName = "TestName";

    finLevData.simEndDay = DAYS_PER_YEAR;
    finLevData.initReservoirLvlMonth = 0;

    bool error_calculated = finLevData.preCheckStartAndEndSim(areaName);
    BOOST_CHECK_EQUAL(true, error_calculated);

    finLevData.simEndDay = DAYS_PER_YEAR - 1;

    error_calculated = finLevData.preCheckStartAndEndSim(areaName);
    BOOST_CHECK_EQUAL(false, error_calculated);

    finLevData.simEndDay = DAYS_PER_YEAR;
    finLevData.initReservoirLvlMonth = 3;

    error_calculated = finLevData.preCheckStartAndEndSim(areaName);
    BOOST_CHECK_EQUAL(false, error_calculated);
}

BOOST_AUTO_TEST_CASE(preCheckYearlyInflow_function_for_area_1)
{
    FinalReservoirLevelRuntimeData& finLevData = area_1->hydro.finalReservoirLevelRuntimeData;

    const AreaName& areaName = "TestName";
    double totalYearInflows = 100;

    finLevData.deltaReservoirLevel = -0.1;
    finLevData.reservoirCapacity = 1500;

    bool error_calculated = finLevData.preCheckYearlyInflow(totalYearInflows, areaName);
    BOOST_CHECK_EQUAL(false, error_calculated);

    finLevData.deltaReservoirLevel = -0.05;

    error_calculated = finLevData.preCheckYearlyInflow(totalYearInflows, areaName);
    BOOST_CHECK_EQUAL(true, error_calculated);
}

BOOST_AUTO_TEST_CASE(preCheckRuleCurves_function_for_area_1)
{
    FinalReservoirLevelRuntimeData& finLevData = area_1->hydro.finalReservoirLevelRuntimeData;

    const AreaName& areaName = "TestName";

    finLevData.finalReservoirLevel = 88;
    finLevData.lowLevelLastDay = 50;
    finLevData.highLevelLastDay = 99;

    bool error_calculated = finLevData.preCheckRuleCurves(areaName);
    BOOST_CHECK_EQUAL(true, error_calculated);

    finLevData.highLevelLastDay = 66;

    error_calculated = finLevData.preCheckRuleCurves(areaName);
    BOOST_CHECK_EQUAL(false, error_calculated);
}

BOOST_AUTO_TEST_CASE(
  Testing_FinalReservoirLevel_function_preChecksPasses_local_variable_equals_true)
{
    // Defining necessary references for areas run time data
    FinalReservoirLevelRuntimeData& finLevDataArea1 = area_1->hydro.finalReservoirLevelRuntimeData;
    FinalReservoirLevelRuntimeData& finLevDataArea2 = area_2->hydro.finalReservoirLevelRuntimeData;

    Parameters& parameters = study->parameters;
    AreaList& areas = study->areas;

    Matrix<double>& scenarioInitialHydroLevels = study->scenarioInitialHydroLevels;
    Matrix<double>& scenarioFinalHydroLevels = study->scenarioFinalHydroLevels;

    FinalReservoirLevel(scenarioInitialHydroLevels, scenarioFinalHydroLevels, parameters, areas);

    BOOST_CHECK(finLevDataArea1.includeFinalReservoirLevel.at(0));
    BOOST_CHECK(finLevDataArea1.includeFinalReservoirLevel.at(1));
    BOOST_CHECK(finLevDataArea2.includeFinalReservoirLevel.at(0));
    BOOST_CHECK(finLevDataArea2.includeFinalReservoirLevel.at(1));

    BOOST_CHECK_EQUAL(finLevDataArea1.endLevel.at(0), scenarioFinalHydroLevels[0][0]);
    BOOST_CHECK_EQUAL(finLevDataArea1.endLevel.at(1), scenarioFinalHydroLevels[0][1]);
    BOOST_CHECK_EQUAL(finLevDataArea2.endLevel.at(0), scenarioFinalHydroLevels[1][0]);
    BOOST_CHECK_EQUAL(finLevDataArea2.endLevel.at(1), scenarioFinalHydroLevels[1][1]);

    BOOST_CHECK_EQUAL(finLevDataArea1.deltaLevel.at(0),
                      (scenarioInitialHydroLevels[0][0] - scenarioFinalHydroLevels[0][0]));
    BOOST_CHECK_EQUAL(finLevDataArea1.deltaLevel.at(1),
                      (scenarioInitialHydroLevels[0][1] - scenarioFinalHydroLevels[0][1]));

    BOOST_CHECK_EQUAL(finLevDataArea2.deltaLevel.at(0),
                      (scenarioInitialHydroLevels[1][0] - scenarioFinalHydroLevels[1][0]));
    BOOST_CHECK_EQUAL(finLevDataArea2.deltaLevel.at(1),
                      (scenarioInitialHydroLevels[1][1] - scenarioFinalHydroLevels[1][1]));
}

BOOST_AUTO_TEST_SUITE_END()*/