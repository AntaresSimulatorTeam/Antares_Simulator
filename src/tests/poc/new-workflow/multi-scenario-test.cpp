#define BOOST_TEST_MODULE poc tests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/dataset.hpp>
#include "antares/optim/api/LinearProblemBuilder.h"
#include "antares/optim/impl/LinearProblemImpl.h"
#include "../include/standard/ComponentFiller.h"

namespace tt = boost::test_tools;
namespace bdata = boost::unit_test::data;
using namespace Antares::optim::api;

static constexpr std::array solverNames = {
  "xpress",
  "sirius",
  "coin",
  "scip"
};

MipSolution runBatterySizingLP(string solverName, vector<int> timeStamps, int timeResolution, vector<unsigned int> scenarios, int batteryLifeTime,
                   vector<vector<double>> consumptionScenarios,vector<vector<double>> thermal1CostScenarios,vector<vector<double>> thermal2CostScenarios )
{
    LinearProblemImpl linearProblem(false, solverName);
    LinearProblemBuilder linearProblemBuilder(linearProblem);
    PortConnectionsManager portConnectionsManager;

    Component balance("balanceA", BALANCE, {}, {{"nodeName", "nodeA"}});
    auto balanceAFiller = make_shared<ComponentFiller>(balance, portConnectionsManager);
    Component priceMinim("priceMinim", PRICE_MINIM, {}, {});
    auto priceMinimFiller = make_shared<ComponentFiller>(priceMinim, portConnectionsManager);
    Component thermal1("thermal1", THERMAL, {{"maxP", 100}}, {});
    auto thermal1Filler = make_shared<ComponentFiller>(thermal1, portConnectionsManager);
    Component thermal2("thermal2", THERMAL, {{"maxP", 100}}, {});
    auto thermal2Filler = make_shared<ComponentFiller>(thermal2, portConnectionsManager);
    Component battery("battery1", BATTERY_WITH_VARIABLE_SIZING,
                      {{"maxP", 9999}, {"maxStock", 9999}, {"lifeTimeInYears", batteryLifeTime}, {"maxStockLifeTimeCost", 150e3}, {"maxPLifeTimeCost", 200e3}}, {});
    auto batteryFiller = make_shared<ComponentFiller>(battery, portConnectionsManager);

    linearProblemBuilder.addFiller(thermal1Filler);
    linearProblemBuilder.addFiller(thermal2Filler);
    linearProblemBuilder.addFiller(batteryFiller);
    linearProblemBuilder.addFiller(balanceAFiller);
    linearProblemBuilder.addFiller(priceMinimFiller);

    portConnectionsManager.addConnection({balanceAFiller, "P"}, {thermal1Filler, "P"});
    portConnectionsManager.addConnection({balanceAFiller, "P"}, {thermal2Filler, "P"});
    portConnectionsManager.addConnection({balanceAFiller, "P"}, {batteryFiller, "P"});
    portConnectionsManager.addConnection({priceMinimFiller, "cost"}, {thermal1Filler, "cost"});
    portConnectionsManager.addConnection({priceMinimFiller, "cost"}, {thermal2Filler, "cost"});
    portConnectionsManager.addConnection({priceMinimFiller, "cost"}, {batteryFiller, "cost"});

    LinearProblemData linearProblemData(timeResolution,
                                        {{"initialStock_battery1", {0}}},
                                        {{"consumption_nodeA", consumptionScenarios},
                                         {"cost_thermal1", thermal1CostScenarios},
                                         {"cost_thermal2", thermal2CostScenarios}});

    BuildContext ctx(scenarios, timeStamps);
    linearProblemBuilder.build(linearProblemData, ctx);

    auto solution = linearProblemBuilder.solve({});

    return solution;
}

BOOST_DATA_TEST_CASE(test_battery_sizing_one_scenario,
                     bdata::make(solverNames),
                     solverName)
{
    // In this test, we will try to find the best investment in a battery
    // This is a copy of test in "standard-test" named "test_std_oneWeek_oneNode_oneBattery_twoThermals",
    // where the battery size (maxP, maxStock) is an optimization variable.
    // The battery costs 150k€/MWh (on maxStock) and 200k€/MW (on maxP), and lives 15 years
    // Under these circumstances, the optimal sizing for the one scenario copied from the existing test
    // should be: maxP = 50 MW, maxStock = 100 MWh, for a total price of 3210.5€ (tested iteratively)

    auto solution = runBatterySizingLP(solverName,
                                       {0, 1, 2, 3},
                                       60,
                                       {0},
                                       15,
                                       {{0, 150, 150, 150}},
                                       {{1, 4, 8, 11}},
                                       {{2, 3, 10, 9}});

    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxP_battery1"), 50);
    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxStock_battery1"), 100);
    BOOST_TEST(solution.getObjectiveValue() == 3210.51, tt::tolerance(0.01));
}

BOOST_DATA_TEST_CASE(test_battery_sizing_three_same_scenarios,
                     bdata::make(solverNames),
                     solverName)
{
    // Copy of previous test, with the same scenario repeated thrice
    // (consumption & thermal prices are scenarized)
    // Expected result is the same as the previous test

    vector<vector<double>> consumptionScenarios =  {{0, 150, 150, 150}, {0, 150, 150, 150}, {0, 150, 150, 150}};
    vector<vector<double>> t1CostScenarios = {{1, 4, 8, 11}, {1, 4, 8, 11}, {1, 4, 8, 11}};
    vector<vector<double>> t2CostScenarios = {{2, 3, 10, 9}, {2, 3, 10, 9}, {2, 3, 10, 9}};

    auto solution = runBatterySizingLP(solverName,
                                       {0, 1, 2, 3},
                                       60,
                                       {0, 1, 2},
                                       15,
                                       consumptionScenarios,
                                       t1CostScenarios,
                                       t2CostScenarios);

    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxP_battery1"), 50);
    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxStock_battery1"), 100);
    BOOST_TEST(solution.getObjectiveValue() == 3210.51, tt::tolerance(0.01));
}


BOOST_DATA_TEST_CASE(test_battery_sizing_multiple_scenarios,
                     bdata::make(solverNames),
                     solverName)
{
    // In this test, we will try to find the best investment in a battery
    // We will study 5 7-hour long scenarios (1-hour step), of a system with 2 thermal units
    // (100 MW each), to which we would like to add a battery.
    // The changing parameters between weeks is the consumption and the thermal costs (because of
    // fuel price fluctuations, for example)
    // The decision variables are: the capacity of the battery (150k€/MWh), its maximum power
    // output/input (200k€/MW).
    // The battery can live 30 years
    // The optimal battery should have 20 MW and 20 MWh (verified iteratively)

    vector<vector<double>> consumptionScenarios = {{0, 150, 150, 150, 0, 30, 200},
                                                   {200, 100, 30, 150, 100, 150, 180},
                                                   {10, 10, 20, 30, 50, 20, 15},
                                                   {200, 190, 200, 200, 180, 190, 200},
                                                   {200, 100, 200, 100, 200, 100, 200}};
    vector<vector<double>> t1CostScenarios = {{1, 4, 8, 11, 8, 4, 1},
                                              {1, 4, 8, 11, 8, 4, 1},
                                              {1, 4, 8, 11, 8, 4, 1},
                                              {10, 10, 11, 10, 10, 11, 10},
                                              {15, 0, 15, 0, 15, 0, 15}};
    vector<vector<double>> t2CostScenarios = {{2, 3, 10, 9, 10, 3, 2},
                                              {10, 0, 0, 8, 0, 0, 10},
                                              {1, 1, 0, 0, 0, 1, 1},
                                              {6, 6, 6, 6, 6, 6, 6},
                                              {9, 9, 9, 9, 9, 9, 9}};

    auto solution = runBatterySizingLP(solverName,
                                       {0, 1, 2, 3, 4, 5, 6},
                                       60,
                                       {0, 1, 2, 3, 4},
                                       30,
                                       consumptionScenarios,
                                       t1CostScenarios,
                                       t2CostScenarios);

    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxP_battery1"), 20);
    BOOST_REQUIRE_EQUAL(solution.getOptimalValue("maxStock_battery1"), 20);
    BOOST_TEST(solution.getObjectiveValue() == 5563.33, tt::tolerance(0.01));
}