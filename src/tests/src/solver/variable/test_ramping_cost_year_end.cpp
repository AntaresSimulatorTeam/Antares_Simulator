// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE "test ramping costs from optimizer ramping values"

#define WIN32_LEAN_AND_MEAN

#include <array>

#include <boost/test/unit_test.hpp>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/variable/economy/operatingCost.h"
#include "antares/solver/variable/economy/rampingCostByDispatchablePlant.h"
#include "antares/solver/variable/economy/rampingCosts.h"
#include "antares/solver/variable/state.h"

#include "in-memory-study.h"

using namespace Antares::Solver::Variable;
using namespace Antares::Solver::Variable::Economy;
using namespace Antares::Data;

namespace
{
constexpr double increaseCost = 3.;
constexpr double decreaseCost = 5.;
constexpr double production = 50.;

// Optimizer ramping variables (P+, P-) for hours 0 to 3. The production is constant from hour 1 on,
// so the former production-difference heuristic would find no ramping at all for these hours.
constexpr std::array<double, 4> powerRampingIncrease = {0., 4., 0., 1.};
constexpr std::array<double, 4> powerRampingDecrease = {0., 0., 2., 2.};

double expectedRampingCost(unsigned hour)
{
    return powerRampingIncrease[hour] * increaseCost + powerRampingDecrease[hour] * decreaseCost;
}

struct StudyWithRampingCluster
{
    explicit StudyWithRampingCluster(UnitCommitmentMode ucMode)
    {
        study = std::make_unique<Study>();
        study->parameters.simulationDays.first = 0;
        study->parameters.simulationDays.end = 7;
        study->maxNbYearsInParallel = 1;
        study->parameters.nbYears = 1;
        // Adequacy avoids the need for a weekly problem, the ramping handoff is mode independent
        study->parameters.mode = SimulationMode::Adequacy;
        study->parameters.unitCommitment.ucMode = ucMode;
        study->parameters.include.thermal_ramping = true;

        area = addAreaToListOfAreas(study->areas, "TestArea");
        area->createMissingData();
        study->areas.rebuildIndexes();
        area->index = 0;

        cluster = addClusterToArea(area, "TestCluster").get();
        cluster->series.timeseriesNumbers.reset(study->parameters.nbYears);
        cluster->nominalCapacityWithSpinning = 100.;
        cluster->unitCount = 1;
        cluster->ramping = ThermalCluster::Ramping();
        cluster->ramping->powerIncreaseCost = increaseCost;
        cluster->ramping->powerDecreaseCost = decreaseCost;

        study->initializeRuntimeInfos();
        study->parameters.resetPlaylist(study->parameters.nbYears);

        results.ProductionThermique.resize(powerRampingIncrease.size());
        for (std::size_t h = 0; h < powerRampingIncrease.size(); ++h)
        {
            auto& hourly = results.ProductionThermique[h];
            hourly.ProductionThermiqueDuPalier = {production};
            hourly.NombreDeGroupesEnMarcheDuPalier = {1.};
            hourly.powerRampingIncrease = {powerRampingIncrease[h]};
            hourly.powerRampingDecrease = {powerRampingDecrease[h]};
        }
    }

    std::unique_ptr<Study> study;
    Area* area = nullptr;
    ThermalCluster* cluster = nullptr;
    RESULTATS_HORAIRES results;
};

// The optimizer ramping values (P+, P-) must reach, unchanged, the per plant ramping cost, the
// area ramping cost and the operating cost, through the hourly state and the year end processing.
// With a constant production the former production-difference heuristic would give 0 everywhere.
void checkOptimizerRampingValuesArePreservedThroughYearEnd(UnitCommitmentMode ucMode)
{
    StudyWithRampingCluster fixture(ucMode);
    auto* area = fixture.area;
    auto* cluster = fixture.cluster;
    const unsigned numSpace = 0;
    const auto nbHours = powerRampingIncrease.size();

    State state(*fixture.study);
    state.area = area;
    state.year = 0;
    state.hourlyResults = &fixture.results;

    RampingCostByDispatchablePlant rampingByPlant;
    rampingByPlant.initializeFromStudy(*fixture.study);
    rampingByPlant.initializeFromArea(fixture.study.get(), area);
    rampingByPlant.yearBegin(0, numSpace);

    // Hourly: optimizer results -> state -> hourly variable
    for (unsigned h = 0; h < nbHours; ++h)
    {
        state.hourInTheWeek = h;
        state.hourInTheYear = h;
        state.hourInTheSimulation = h;
        state.initFromThermalClusterIndex(cluster->enabledIndex);

        BOOST_CHECK_CLOSE(state.thermal[area->index].thermalClustersRampingCost[0]
                            + 1., // avoids a relative comparison with 0
                          expectedRampingCost(h) + 1.,
                          1e-9);
        // No fuel, startup or fixed cost: the hourly operating cost is the ramping cost
        BOOST_CHECK_CLOSE(state.thermal[area->index].thermalClustersOperatingCost[0] + 1.,
                          expectedRampingCost(h) + 1.,
                          1e-9);
        rampingByPlant.hourForEachArea(state, numSpace);
    }

    // Year end
    state.yearEndResetThermal();
    state.thermalCluster = cluster;
    for (unsigned h = 0; h < nbHours; ++h)
    {
        state.thermalClusterProductionForYear[h] = production;
    }

    rampingByPlant.yearEndBuildPrepareDataForEachThermalCluster(state, 0, numSpace);
    for (unsigned h = 0; h < nbHours; ++h)
    {
        BOOST_CHECK_CLOSE(state.thermalClusterRampingCostForYear[h] + 1.,
                          expectedRampingCost(h) + 1.,
                          1e-9);
    }

    state.yearEndBuildFromThermalClusterIndex(cluster->enabledIndex);

    RampingCost areaRamping;
    areaRamping.initializeFromStudy(*fixture.study);
    areaRamping.initializeFromArea(fixture.study.get(), area);
    areaRamping.yearBegin(0, numSpace);
    areaRamping.yearEndBuildForEachThermalCluster(state, 0, numSpace);

    OperatingCost operatingCost;
    operatingCost.initializeFromStudy(*fixture.study);
    operatingCost.initializeFromArea(fixture.study.get(), area);
    operatingCost.yearBegin(0, numSpace);
    operatingCost.yearEndBuildForEachThermalCluster(state, 0, numSpace);

    rampingByPlant.yearEndBuildForEachThermalCluster(state, 0, numSpace);

    const double* perPlant = rampingByPlant.retrieveRawHourlyValuesForCurrentYear(0, numSpace);
    const double* perArea = areaRamping.retrieveRawHourlyValuesForCurrentYear(0, numSpace);
    const double* operating = operatingCost.retrieveRawHourlyValuesForCurrentYear(0, numSpace);
    BOOST_REQUIRE(perPlant && perArea && operating);

    // The operating cost of the plant is only made of its ramping cost here (no fuel/fixed cost)
    for (unsigned h = 0; h < nbHours; ++h)
    {
        BOOST_CHECK_CLOSE(perPlant[h] + 1., expectedRampingCost(h) + 1., 1e-9);
        BOOST_CHECK_CLOSE(perArea[h] + 1., expectedRampingCost(h) + 1., 1e-9);
        BOOST_CHECK_CLOSE(operating[h] + 1., expectedRampingCost(h) + 1., 1e-9);
    }
}
} // anonymous namespace

BOOST_AUTO_TEST_SUITE(RampingCostFromOptimizerTests)

BOOST_AUTO_TEST_CASE(accurate_mode_preserves_optimizer_ramping_values_through_year_end)
{
    checkOptimizerRampingValuesArePreservedThroughYearEnd(UnitCommitmentMode::ucHeuristicAccurate);
}

BOOST_AUTO_TEST_CASE(milp_mode_preserves_optimizer_ramping_values_through_year_end)
{
    checkOptimizerRampingValuesArePreservedThroughYearEnd(UnitCommitmentMode::ucMILP);
}

BOOST_AUTO_TEST_SUITE_END()
