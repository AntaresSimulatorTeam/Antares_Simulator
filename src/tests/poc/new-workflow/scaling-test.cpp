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
using namespace std;

static constexpr std::array solverNames =
        {
                "xpress",
                "sirius",
                "coin",
                //"scip" // TODO activate this after adding tolerance
        };

static constexpr std::array timestepNumbers =
        {
                24, // 1 day
                168, // 1 week
                744, // 1 month
                4380 // 6 months
        };

BOOST_DATA_TEST_CASE(test_scaling_simple_problem, bdata::make(solverNames)*bdata::make(timestepNumbers), solverName, nTimesteps)
{
    int timeResolution = 60;

    // Build timesteps vector beginning at 0
    std::vector<int> timeStamps(nTimesteps);
    std::iota(timeStamps.begin(), timeStamps.end(), 0);

    LinearProblemData::TimedDataDict timedData;

    // Build increasing consumption vector beginning at 0 MW, and increasing by 1 MW at every timestep
    std::vector<double> consumption(nTimesteps);
    std::iota(consumption.begin(), consumption.end(), 1);
    timedData.insert({"consumption_nodeA", consumption});

    LinearProblemImpl linearProblem(false, solverName);
    LinearProblemBuilder linearProblemBuilder(linearProblem);
    PortConnectionsManager portConnectionsManager;

    Component balance("balanceA", BALANCE, {}, {{"nodeName", "nodeA"}});
    shared_ptr<ComponentFiller> balanceAFiller = make_shared<ComponentFiller>(balance, portConnectionsManager);
    linearProblemBuilder.addFiller(balanceAFiller);

    Component priceMinim("priceMinim", PRICE_MINIM, {}, {});
    shared_ptr<ComponentFiller> priceMinimFiller = make_shared<ComponentFiller>(priceMinim, portConnectionsManager);
    linearProblemBuilder.addFiller(priceMinimFiller);

    // Create thermal production units
    // Every unit can produce up to 10 MW
    // Price is stable in time but increase for every unit
    int nUnits = ceil(1.0 * (nTimesteps - 1) / 10.0);
    for (int i = 1; i <= nUnits; ++i) {
        string id = "thermal" + to_string(i);
        Component thermal(id, THERMAL, {{"maxP", 10}}, {});
        shared_ptr<ComponentFiller> thermalFiller = make_shared<ComponentFiller>(thermal, portConnectionsManager);
        vector<double> cost(nTimesteps, 1.0 * i);
        timedData.insert({"cost_" + id, cost});
        linearProblemBuilder.addFiller(thermalFiller);
        portConnectionsManager.addConnection({balanceAFiller, "P"}, {thermalFiller, "P"});
        portConnectionsManager.addConnection({priceMinimFiller, "cost"}, {thermalFiller, "cost"});
    }

    LinearProblemData linearProblemData(timeStamps, timeResolution, {}, timedData);
    linearProblemBuilder.build(linearProblemData);
    auto solution = linearProblemBuilder.solve({});

    for (int i = 1; i <= nUnits; ++i) {
        string pVarNamePrefix  = "P_thermal" + to_string(i) + "_";
        vector<string> pVarNames;
        pVarNames.reserve(nTimesteps);
        vector<double> expectedP;
        expectedP.reserve(nTimesteps);
        for (int ts: timeStamps) {
            pVarNames.push_back(pVarNamePrefix + to_string(ts));
            expectedP.push_back(min(10.0, max(0.0, consumption[ts] - (i - 1) * 10)));
        }
        vector<double> actualP = solution.getOptimalValues(pVarNames);
        BOOST_TEST(actualP == expectedP, tt::per_element()); // TODO add tolerance
    }
}