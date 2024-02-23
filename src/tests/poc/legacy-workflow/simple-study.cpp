#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "../../../solver/optimisation/opt_global.h"
#include "../include/simple/Thermal.h"
#include "../include/simple/Balance.h"

#include "utils.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

using namespace Antares::Data;

// =================================
// Basic fixture 
// =================================
struct StudyFixture : public StudyBuilder
{
	using StudyBuilder::StudyBuilder;
	StudyFixture();

	// Data members
	std::shared_ptr<ThermalCluster> cluster;
	Area* area = nullptr;
	double loadInArea = 0.;
	TimeSeriesConfigurer loadTSconfig;
};

StudyFixture::StudyFixture()
{
	simulationBetweenDays(0, 7);
	area = addAreaToStudy("some_area");

	loadInArea = 7.0;
	loadTSconfig = TimeSeriesConfigurer(area->load.series.timeSeries);
	loadTSconfig.setColumnCount(1)
				.fillColumnWith(0, loadInArea);
};


BOOST_FIXTURE_TEST_SUITE(ONE_AREA__ONE_THERMAL_CLUSTER, StudyFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_fullfills_area_demand)
{	
	setNumberMCyears(1);

    std::vector<int> timeStamps(168);
    std::iota(timeStamps.begin(),
              timeStamps.end(),
              0);
    {
        std::vector<double> costThermal1(168, 1.);
        std::vector<double> consumption1(168, 100);
        gLinearProblemData = Antares::optim::api::LinearProblemData(timeStamps,
                                                                    60, // timeResolutionInMinutes
                                                                    {}, // scalarData
                                                                    {{"cost_thermal1", std::move(costThermal1)},
                                                                     {"consumption_some_area", consumption1}});

        auto thermal = new Thermal("thermal1", 100);
        gAdditionalFillers.push_back(thermal);
        gAdditionalFillers.push_back(new Balance("some_area", {}, {thermal}));
    }

    area->thermal.unsuppliedEnergyCost = 1.e3;
    area->thermal.spilledEnergyCost = 1.;

    // VERY IMPORTANT
    study->parameters.ortoolsUsed = true;

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    //BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
    std::vector<double> expectedThermalP(168, loadInArea);

    std::vector<std::string> query;
    for (int timeStamp : timeStamps)
       query.push_back("P_thermal1_"+std::to_string(timeStamp));
    auto actualThermalP = gMipSolution.getOptimalValues(query);

    // Test no unsupplied energy at h=0
    BOOST_TEST(output.unsupliedEnergy(area).hour(0) == 0, tt::tolerance(1.e-6));

    BOOST_TEST(actualThermalP == expectedThermalP, tt::per_element()); // TODO add tolerance
}
BOOST_AUTO_TEST_SUITE_END()



