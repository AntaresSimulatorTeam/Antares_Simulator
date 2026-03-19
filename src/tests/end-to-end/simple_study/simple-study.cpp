// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE test - end - to - end tests
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

#include "in-memory-study.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

using namespace Antares::Data;

// =================================
// Basic fixture
// =================================
struct StudyFixture: public StudyBuilder
{
    StudyFixture();

    // Data members
    Area* area = nullptr;
    std::shared_ptr<ThermalCluster> cluster;
    double loadInArea = 0.;
    double clusterCost = 0.;
    ThermalClusterConfig clusterConfig;
    TimeSeriesConfigurer loadTSconfig;
};

StudyFixture::StudyFixture():
    area(addAreaToStudy("Some area")),
    cluster(addClusterToArea(area, "some cluster")),
    clusterConfig(cluster),
    loadTSconfig(area->load.series)
{
    simulationBetweenDays(0, 7);
    loadInArea = 7.0;
    loadTSconfig.setDimensions(1).fillColumnWith(0, loadInArea);

    clusterCost = 2.;
    clusterConfig.setNominalCapacity(100.)
      .setAvailablePower(0, 50.)
      .setCosts(clusterCost)
      .setUnitCount(1);
}

struct HydroMaxPowerStudy: public StudyBuilder
{
    using StudyBuilder::StudyBuilder;
    HydroMaxPowerStudy();

    // Data members
    Area* area = nullptr;
    PartHydro* hydro = nullptr;
    double loadInArea = 24000.;
};

HydroMaxPowerStudy::HydroMaxPowerStudy()
{
    simulationBetweenDays(0, 14);

    area = addAreaToStudy("Area");
    area->thermal.unsuppliedEnergyCost = 1;

    setNumberMCyears(1);

    TimeSeriesConfigurer loadTSconfig(area->load.series);
    loadTSconfig.setDimensions(1).fillColumnWith(0, loadInArea);

    hydro = &area->hydro;

    TimeSeriesConfigurer genP(hydro->series->maxHourlyGenPower);
    genP.setDimensions(1).fillColumnWith(0, 100.);

    TimeSeriesConfigurer hydroStorage(hydro->series->storage);
    hydroStorage.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 2400.);

    TimeSeriesConfigurer genE(hydro->dailyNbHoursAtGenPmax);
    genE.setDimensions(1, DAYS_PER_YEAR).fillColumnWith(0, 24);

    hydro->reservoirCapacity = 1e6;
    hydro->reservoirManagement = true;
}

BOOST_AUTO_TEST_SUITE(ONE_AREA__ONE_THERMAL_CLUSTER)

BOOST_FIXTURE_TEST_CASE(thermal_cluster_fullfills_area_demand, StudyFixture)
{
    setNumberMCyears(1);
    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(two_MC_years__thermal_cluster_fullfills_area_demand_on_2nd_year_as_well,
                        StudyFixture)
{
    setNumberMCyears(2);

    simulation.create();
    playOnlyYear(1);
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(two_mc_years__two_ts_identical, StudyFixture)
{
    setNumberMCyears(2);

    loadTSconfig.setDimensions(2).fillColumnWith(0, 7.0).fillColumnWith(1, 7.0);

    clusterConfig.setAvailablePowerNumberOfTS(2).setAvailablePower(0, 50.).setAvailablePower(1,
                                                                                             50.);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(two_mc_years__two_ts_for_load, StudyFixture)
{
    setNumberMCyears(2);

    loadTSconfig.setDimensions(2).fillColumnWith(0, 7.0).fillColumnWith(1, 14.0);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
    scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    double averageLoad = (7. + 14.) / 2.;
    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == averageLoad,
               tt::tolerance(0.001));
    BOOST_TEST(output.overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(two_mc_years_with_different_weight__two_ts, StudyFixture)
{
    setNumberMCyears(2);

    giveWeightToYear(4.f, 0);
    giveWeightToYear(10.f, 1);
    float weightSum = study->parameters.getYearsWeightSum();

    loadTSconfig.setDimensions(2).fillColumnWith(0, 7.0).fillColumnWith(1, 14.0);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
    scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    double averageLoad = (4 * 7. + 10. * 14.) / weightSum;
    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == averageLoad,
               tt::tolerance(0.001));
    BOOST_TEST(output.overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(milp_two_mc_single_unit_single_scenario, StudyFixture)
{
    setNumberMCyears(1);

    // Arbitrary large number, only characteristic is : larger than all
    // other marginal costs
    area->thermal.unsuppliedEnergyCost = 1000;

    // Use OR-Tools / COIN for MILP
    auto& p = study->parameters;
    p.unitCommitment.ucMode = ucMILP;
    p.optOptions.firstOptimOptions.solverName = "coin";
    p.optOptions.secondOptimOptions.solverName = "coin";

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());

    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea,
               tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 1, tt::tolerance(0.001));
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(milp_two_mc_two_unit_single_scenario, StudyFixture)
{
    setNumberMCyears(1);

    clusterConfig.setAvailablePower(0, 150.).setUnitCount(2);

    loadInArea = 150;
    loadTSconfig.setDimensions(1).fillColumnWith(0, loadInArea);
    // Arbitrary large number, only characteristic is : larger than all
    // other marginal costs
    area->thermal.unsuppliedEnergyCost = 1000;

    // Use OR-Tools / COIN for MILP
    auto& p = study->parameters;
    p.unitCommitment.ucMode = ucMILP;
    p.optOptions.firstOptimOptions.solverName = "coin";
    p.optOptions.secondOptimOptions.solverName = "coin";

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());

    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea,
               tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 2, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(parallel, StudyFixture)
{
    setNumberMCyears(10);
    study->maxNbYearsInParallel = 2;

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(parallel2, StudyFixture)
{
    setNumberMCyears(2);
    study->maxNbYearsInParallel = 2;

    loadTSconfig.setDimensions(2).fillColumnWith(0, 7.0).fillColumnWith(1, 7.0);

    clusterConfig.setAvailablePowerNumberOfTS(2).setAvailablePower(0, 50.).setAvailablePower(1,
                                                                                             50.);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(error_cases)

BOOST_FIXTURE_TEST_CASE(error_on_wrong_hydro_data, StudyFixture)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 7);
    Area& area = *builder.addAreaToStudy("A");
    PartHydro& hydro = area.hydro;
    TimeSeriesConfigurer(hydro.series->storage)
      .setDimensions(1)
      .fillColumnWith(0, -1.0); // Negative inflow will cause a consistency error with mingen

    builder.setNumberMCyears(1);
    auto& simulation = builder.simulation;
    simulation.create();
    BOOST_CHECK_THROW(simulation.run(), Antares::FatalError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ONE_AREA__ONE_STS_THERMAL_CLUSTER)

BOOST_FIXTURE_TEST_CASE(STS_initial_level_is_also_weekly_final_level, StudyFixture)
{
    setNumberMCyears(1);
    auto* sts = addSTSToArea(area, "my-sts");

    const double initialLevel = .443;
    const double reservoirCapacity = 100;

    ShortTermStorageConfig stsConfig(*sts);
    stsConfig.setInjectionNominalCapacity(10)
      .setWithdrawalNominalCapacity(10)
      .setReservoirCapacity(reservoirCapacity)
      .setInjectionEfficiency(.9)
      .setWithdrawalEfficiency(.8)
      .setInitialLevel(initialLevel)
      .setInitialLevelOptim(false)
      .setGroupName("Some STS group");
    // Default values for series
    sts->series->fillDefaultSeriesIfEmpty();

    auto ct = stsConfig.addConstraint()
                .setName("my-constraint")
                .setVariable("withdrawal")
                .setOperatorType("equal")
                .setHours({{1, 2, 3}}) // first hour has index 1
                .build();

    TimeSeriesConfigurer addcRHS(ct->rhs());
    addcRHS.setDimensions(1).fillColumnWith(0, 4.);

    // Fatal gen at h=1
    auto& windTS = area->wind.series;
    TimeSeriesConfigurer(windTS).setDimensions(1).fillColumnWith(0, 0.);
    windTS[0][1] = 100;

    // Fatal load at h=2-10
    auto& loadTS = area->load.series;
    TimeSeriesConfigurer(loadTS).setDimensions(1).fillColumnWith(0, 0.);
    for (int i = 2; i < 10; i++)
    {
        loadTS[0][i] = 100;
    }

    // Usual values, avoid spillage & unsupplied energy
    area->thermal.unsuppliedEnergyCost = 1.e3;
    area->thermal.spilledEnergyCost = 1.;

    simulation.create();
    simulation.run();

    unsigned int groupNb = 0; // Used to reach the first group of STS results
    OutputRetriever output(simulation.rawSimu());

    // Check the initial level
    BOOST_TEST(output.levelForSTSgroup(area, groupNb).hour(167) == initialLevel * reservoirCapacity,
               tt::tolerance(0.001));

    // Check that the withdrawal additional constraint is taken into account
    auto withdrawal = output.withdrawalForSTSgroup(area, groupNb);
    BOOST_TEST(withdrawal.hour(0) + withdrawal.hour(1) + withdrawal.hour(2) == 4. + 4. + 4.,
               tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(STS_efficiency_for_injection_and_withdrawal, StudyFixture)
{
    setNumberMCyears(1);

    auto* sts = addSTSToArea(area, "my-sts");

    const double initialLevel = .5;
    const double reservoirCapacity = 100;

    ShortTermStorageConfig stsConfig(*sts);
    stsConfig.setInjectionNominalCapacity(10)
      .setWithdrawalNominalCapacity(10)
      .setReservoirCapacity(reservoirCapacity)
      .setInjectionEfficiency(.6)
      .setWithdrawalEfficiency(.8)
      .setInitialLevel(initialLevel)
      .setInitialLevelOptim(false)
      .setGroupName("Some STS group");
    // Default values for series
    sts->series->fillDefaultSeriesIfEmpty();
    sts->series->costLevel.assign(HOURS_PER_YEAR, 0);

    // Fatal gen at h=1
    auto& windTS = area->wind.series;
    TimeSeriesConfigurer(windTS).setDimensions(1).fillColumnWith(0, 0.);
    windTS[0][1] = 100;

    // Fatal load at h=2
    auto& loadTS = area->load.series;
    TimeSeriesConfigurer(loadTS).setDimensions(1).fillColumnWith(0, 0.);
    loadTS[0][2] = 100;

    // Usual values, avoid spillage & unsupplied energy
    area->thermal.unsuppliedEnergyCost = 1.e3;
    area->thermal.spilledEnergyCost = 1.;

    simulation.create();
    simulation.run();

    unsigned int groupNb = 0; // Used to reach the first group of STS results
    OutputRetriever output(simulation.rawSimu());

    BOOST_CHECK_EQUAL(output.levelForSTSgroup(area, groupNb).hour(1), 56); // injection
    BOOST_CHECK_EQUAL(output.levelForSTSgroup(area, groupNb).hour(2), 48); // withdrawal
}

BOOST_FIXTURE_TEST_CASE(sts_scenarized_withdrawal_constraint, StudyBuilder)
{
    simulationBetweenDays(0, 7);
    Area* area = addAreaToStudy("A");
    auto* sts = addSTSToArea(area, "my-sts");
    ShortTermStorageConfig stsConfig(*sts);
    stsConfig.setReservoirCapacity(1000)
      .setWithdrawalNominalCapacity(10)
      .setInjectionNominalCapacity(10)
      .setInitialLevelOptim(true);

    sts->series->fillDefaultSeriesIfEmpty();

    auto ct = stsConfig.addConstraint()
                .setName("my-constraint")
                .setVariable("withdrawal")
                .setOperatorType("equal")
                .setHours({{1, 2}}) // first hour has index 1
                .build();

    const int nbYears = 4;
    const int nbHours = 2; // Constraint on 2 hours
    const double additionalConstraint_RHS[] = {1., 2., 4., 8.};
    TimeSeriesConfigurer addcRHS(ct->rhs());
    addcRHS.setDimensions(nbYears)
      .fillColumnWith(0, additionalConstraint_RHS[0])
      .fillColumnWith(1, additionalConstraint_RHS[1])
      .fillColumnWith(2, additionalConstraint_RHS[2])
      .fillColumnWith(3, additionalConstraint_RHS[3]);

    setNumberMCyears(nbYears);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    auto& stsScenario = scenarioBuilderRule.stsAdditionalConstraints()[/*areaIndex = */ 0];

    stsScenario.setTSnumber(ct.get(), 0, 1);
    stsScenario.setTSnumber(ct.get(), 1, 2);
    stsScenario.setTSnumber(ct.get(), 2, 3);
    stsScenario.setTSnumber(ct.get(), 3, 4);

    const double expectedAverage = nbHours
                                   * (additionalConstraint_RHS[0] + additionalConstraint_RHS[1]
                                      + additionalConstraint_RHS[2] + additionalConstraint_RHS[3])
                                   / nbYears;

    simulation.create();
    simulation.run();

    unsigned int groupNb = 0; // Used to reach the first group of STS results
    OutputRetriever output(simulation.rawSimu());

    auto withdrawal = output.withdrawalForSTSgroup(area, groupNb);
    BOOST_TEST(withdrawal.hour(0) + withdrawal.hour(1) == expectedAverage, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(IN_MEMORY_PROBLEM_BUILDING)

BOOST_FIXTURE_TEST_CASE(STS_efficiency_for_injection_and_withdrawal, StudyFixture)
{
    setNumberMCyears(1);

    auto* sts = addSTSToArea(area, "my-sts");

    const double initialLevel = .5;
    const double reservoirCapacity = 100;

    ShortTermStorageConfig stsConfig(*sts);
    stsConfig.setInjectionNominalCapacity(10)
      .setWithdrawalNominalCapacity(10)
      .setReservoirCapacity(reservoirCapacity)
      .setInjectionEfficiency(.6)
      .setWithdrawalEfficiency(.8)
      .setInitialLevel(initialLevel)
      .setInitialLevelOptim(false)
      .setGroupName("Some STS group");
    // Default values for series
    sts->series->fillDefaultSeriesIfEmpty();

    simulation.create();
    simulation.run();

    const auto& observer = simulation.getObserver();
    BOOST_REQUIRE_EQUAL(observer.problems.size(), 2);
    BOOST_REQUIRE(observer.problems.contains({2, "problem-1-1--optim-nb-2.mps"}));
    const auto& problem = observer.problems.at({2, "problem-1-1--optim-nb-2.mps"});

    // Withdrawal variable
    const std::string withdrawalKey = "Withdrawal::area<some*area>::ShortTermStorage<my-sts>::hour<"
                                      "38>";
    BOOST_REQUIRE(problem.variables.contains(withdrawalKey));

    const auto& withdrawal_h38 = problem.variables.at(withdrawalKey);
    BOOST_CHECK_EQUAL(withdrawal_h38.Xmin, 0);
    BOOST_CHECK_EQUAL(withdrawal_h38.Xmax, 10);
    BOOST_CHECK_EQUAL(withdrawal_h38.objectiveCoefficient, 0);

    // Level constraint
    const std::string levelKey = "Level::area<some*area>::ShortTermStorage<my-sts>::hour<38>";
    const std::string injectionKey = "Injection::area<some*area>::ShortTermStorage<my-sts>::hour<"
                                     "38>";
    BOOST_REQUIRE(problem.constraints.contains(levelKey));
    const auto& level_h38 = problem.constraints.at(levelKey);
    BOOST_CHECK_EQUAL(level_h38.coefficients.at(withdrawalKey), 0.8); // withdrawalEfficiency
    BOOST_CHECK_EQUAL(level_h38.coefficients.at(injectionKey), -0.6); // -injectionEfficiency
    BOOST_CHECK_EQUAL(level_h38.rhs, 0);                              // no inflows

    const std::string overflowKey
      = "Overflow::area<some*area>::ShortTermStorage<my-sts>::hour<38>"; // Overflow variable
    BOOST_CHECK(!level_h38.coefficients.contains(
      overflowKey)); // check that "overflow" variable does not exist
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(sts_overflow)

BOOST_FIXTURE_TEST_CASE(overflow_exists_and_has_right_coeff, StudyFixture)
{
    setNumberMCyears(1);

    auto* sts = addSTSToArea(area, "my-sts");

    const double initialLevel = .5;
    const double reservoirCapacity = 100;

    ShortTermStorageConfig stsConfig(*sts);
    stsConfig.setInjectionNominalCapacity(10)
      .setWithdrawalNominalCapacity(10)
      .setReservoirCapacity(reservoirCapacity)
      .setInjectionEfficiency(.6)
      .setWithdrawalEfficiency(.8)
      .setInitialLevel(initialLevel)
      .setInitialLevelOptim(false)
      .setAllowOverflow(true) // Allow overflows
      .setGroupName("Some STS group");
    // Default values for series
    sts->series->fillDefaultSeriesIfEmpty();

    simulation.create();
    simulation.run();

    const auto& observer = simulation.getObserver();
    BOOST_REQUIRE_EQUAL(observer.problems.size(), 2);
    BOOST_REQUIRE(observer.problems.contains({2, "problem-1-1--optim-nb-2.mps"}));
    const auto& problem = observer.problems.at({2, "problem-1-1--optim-nb-2.mps"});

    const std::string levelKey
      = "Level::area<some*area>::ShortTermStorage<my-sts>::hour<38>"; // Level constraint
    const std::string overflowKey
      = "Overflow::area<some*area>::ShortTermStorage<my-sts>::hour<38>"; // Overflow variable
    BOOST_REQUIRE(problem.constraints.contains(levelKey));
    const auto& level_h38 = problem.constraints.at(levelKey);
    BOOST_CHECK_EQUAL(level_h38.coefficients.at(overflowKey),
                      1.0); // check that "overflow" variable exists and has the right coefficient
    auto ovf = problem.variables.find(overflowKey);
    BOOST_REQUIRE(ovf != problem.variables.end());
    BOOST_CHECK_EQUAL(ovf->second.objectiveCoefficient,
                      area->thermal.spilledEnergyCost + area->hydro.overflowSpilledCostDifference);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(HYDRO_MAX_POWER)

BOOST_FIXTURE_TEST_CASE(basic, HydroMaxPowerStudy)
{
    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());

    BOOST_TEST(output.hydroStorage(area).hour(0) == hydro->series->maxHourlyGenPower[0][0],
               tt::tolerance(0.001));
    BOOST_TEST(output.overallCost(area).hour(0)
                 == (loadInArea - output.hydroStorage(area).hour(0))
                      * area->thermal.unsuppliedEnergyCost,
               tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(scenario_builder, HydroMaxPowerStudy)
{
    setNumberMCyears(3);

    giveWeightToYear(4.f, 0);
    giveWeightToYear(3.f, 1);
    giveWeightToYear(2.f, 2);
    float weightSum = study->parameters.getYearsWeightSum();

    TimeSeriesConfigurer genP(hydro->series->maxHourlyGenPower);
    TimeSeriesConfigurer genE(hydro->series->maxHourlyPumpPower);
    genP.setDimensions(3).fillColumnWith(0, 100.).fillColumnWith(1, 200.).fillColumnWith(2, 300.);
    genE.setDimensions(3).fillColumnWith(0, 0.).fillColumnWith(1, 0.).fillColumnWith(2, 0.);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.hydro().setTSnumber(area->index, 0, 3);
    scenarioBuilderRule.hydro().setTSnumber(area->index, 1, 2);
    scenarioBuilderRule.hydro().setTSnumber(area->index, 2, 1);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());

    double averageLoad = (4 * 300. + 3. * 200. + 2. * 100.) / weightSum;

    BOOST_TEST(output.overallCost(area).hour(0)
                 == loadInArea - averageLoad * area->thermal.unsuppliedEnergyCost,
               tt::tolerance(0.1));
}

BOOST_AUTO_TEST_SUITE_END()
