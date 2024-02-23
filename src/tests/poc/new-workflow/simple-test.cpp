#define BOOST_TEST_MODULE poc tests
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/dataset.hpp>
#include "antares/optim/api/LinearProblemBuilder.h"
#include "antares/optim/impl/LinearProblemImpl.h"
#include "../include/simple/Battery.h"
#include "../include/simple/Thermal.h"
#include "../include/simple/Balance.h"
#include "../include/simple/ProductionPriceMinimization.h"

namespace tt = boost::test_tools;
namespace bdata = boost::unit_test::data;
using namespace Antares::optim::api;

static const std::string solverNames[] =
        {
                "xpress",
                //"sirius", // TODO fix this
                "coin",
                //"glpk", // TODO fix this
                //"scip" // TODO activate this after adding tolerance
        };

BOOST_DATA_TEST_CASE(test_oneWeek_oneNode_oneBattery_oneThermal,
                     bdata::make(solverNames), solverName)
{
    vector<int> timeStamps{0, 1, 2, 3};
    int timeResolution = 60;

    LinearProblemImpl linearProblem(false, solverName);
    LinearProblemBuilder linearProblemBuilder(linearProblem);

    Battery battery("battery1", 100, 1000);
    Thermal thermal("thermal1", 100);
    Balance balance("nodeA", {&battery}, {&thermal});
    ProductionPriceMinimization objective({&thermal});

    linearProblemBuilder.addFiller(battery);
    linearProblemBuilder.addFiller(thermal);
    linearProblemBuilder.addFiller(balance);
    linearProblemBuilder.addFiller(objective);

    LinearProblemData linearProblemData(
            timeStamps, // TODO : move to LinearProblem ?
            timeResolution, // TODO : move to LinearProblem ?
            {
                    {"initialStock_battery1", 0}
            },
            {
                    {"consumption_nodeA", {50, 50, 150, 120}},
                    {"cost_thermal1",     {1,  3,  10,  8}}
            });
    linearProblemBuilder.build(linearProblemData);
    auto solution = linearProblemBuilder.solve({});

    // Consumption is greater than thermal maximum production in TS 2 & 3
    // So, the battery has to charge during tS 0 & 1
    // Moreover, production cost is big during TS 2 & 3, so the battery has to charge up to its maximum during 0 & 1
    // Consumption in 0 & 1 is 50 MW, so the battery can charge 50 MW x 2
    // It must then discharge in 2 & 3, but mostly in 2 because thermal production cost is higher, while keeping 20 MW
    // to achieve balance in 3 (consumption = 120, thermal production <= 100)
    // Thermal production must complete the rest. Thus, the expected power plans are:
    // Thermal : 100, 100, 70, 100
    // Battery : -50, -50, 80, 20

    vector<double> actualThermalP = solution.getOptimalValues({"P_thermal1_0", "P_thermal1_1", "P_thermal1_2", "P_thermal1_3"});
    vector<double> expectedThermalP({100., 100., 70., 100.});
    BOOST_TEST(actualThermalP == expectedThermalP, tt::per_element()); // TODO add tolerance

    vector<double> actualBatteryP = solution.getOptimalValues({"P_battery1_0", "P_battery1_1", "P_battery1_2", "P_battery1_3"});
    vector<double> expectedBatteryP({-50, -50, 80, 20});
    BOOST_TEST(actualBatteryP == expectedBatteryP, tt::per_element()); // TODO add tolerance
}

BOOST_DATA_TEST_CASE(test_oneWeek_oneNode_oneBattery_twoThermals,
                     bdata::make(solverNames), solverName)
{
    vector<int> timeStamps{0, 1, 2, 3};
    int timeResolution = 60;

    LinearProblemImpl linearProblem(false, solverName);
    LinearProblemBuilder linearProblemBuilder(linearProblem);

    Battery battery1("battery1", 180, 200);
    Thermal thermal1("thermal1", 100);
    Thermal thermal2("thermal2", 100);
    Balance balance("nodeA", {&battery1}, {&thermal1, &thermal2});
    ProductionPriceMinimization objective({&thermal1, &thermal2});

    linearProblemBuilder.addFiller(battery1);
    linearProblemBuilder.addFiller(thermal1);
    linearProblemBuilder.addFiller(thermal2);
    linearProblemBuilder.addFiller(balance);
    linearProblemBuilder.addFiller(objective);

    LinearProblemData linearProblemData(
            timeStamps, // TODO : move to LinearProblem ?
            timeResolution, // TODO : move to LinearProblem ?
            {
                    {"initialStock_battery1", 0}
            },
            {
                    {"consumption_nodeA", {0, 150, 150, 150}},
                    {"cost_thermal1",     {1,  4,  8,  11}},
                    {"cost_thermal2",     {2,  3,  10,  9}}
            });
    linearProblemBuilder.build(linearProblemData);
    auto solution = linearProblemBuilder.solve({});

    // Thermal production is cheap in TS 0, then very expensive.
    // Battery must charge up to its max during TS 0, then discharge mostly when thermal production is most
    // expensive (mostly in TS 3, then in TS 2). It is limited in power and stock, it can charge 180 MW in TS 0 and
    // 20 MW in TS 2.
    // Thermal production will complete the rest, following merit order imposed by their respective costs.
    // Expected power plans are:
    // - Battery: -180, -20, 50, 150
    // - Thermal1: 100, 70, 100, 0
    // - Thermal2: 80, 100, 0, 0

    vector<double> actualBatteryP = solution.getOptimalValues({"P_battery1_0", "P_battery1_1", "P_battery1_2", "P_battery1_3"});
    vector<double> expectedBatteryP({-180, -20, 50, 150});
    BOOST_TEST(actualBatteryP == expectedBatteryP, tt::per_element()); // TODO add tolerance

    vector<double> actualThermal1P = solution.getOptimalValues({"P_thermal1_0", "P_thermal1_1", "P_thermal1_2", "P_thermal1_3"});
    vector<double> expectedThermal1P({100, 70, 100, 0});
    BOOST_TEST(actualThermal1P == expectedThermal1P, tt::per_element()); // TODO add tolerance
    vector<double> actualThermal2P = solution.getOptimalValues({"P_thermal2_0", "P_thermal2_1", "P_thermal2_2", "P_thermal2_3"});
    vector<double> expectedThermal2P({80, 100, 0, 0});
    BOOST_TEST(actualThermal2P == expectedThermal2P, tt::per_element()); // TODO add tolerance
}
