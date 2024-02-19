#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include "../../../solver/optimisation/opt_global.h"
#include "../include/simple/Thermal.h"

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
	double clusterCost = 0.;
	ThermalClusterConfig clusterConfig;
	TimeSeriesConfigurer loadTSconfig;
};

StudyFixture::StudyFixture()
{
	simulationBetweenDays(0, 7);
	area = addAreaToStudy("Some area");
	cluster = addClusterToArea(area, "some cluster");

	loadInArea = 7.0;
	loadTSconfig = TimeSeriesConfigurer(area->load.series.timeSeries);
	loadTSconfig.setColumnCount(1)
				.fillColumnWith(0, loadInArea);

	clusterCost = 2.;
	clusterConfig = ThermalClusterConfig(cluster.get());
	clusterConfig.setNominalCapacity(100.)
				 .setAvailablePower(0, 50.)
				 .setCosts(clusterCost)
				 .setUnitCount(1);
};


BOOST_FIXTURE_TEST_SUITE(ONE_AREA__ONE_THERMAL_CLUSTER, StudyFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_fullfills_area_demand)
{	
	setNumberMCyears(1);

    gLinearProblemData = 

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}
BOOST_AUTO_TEST_SUITE_END()



