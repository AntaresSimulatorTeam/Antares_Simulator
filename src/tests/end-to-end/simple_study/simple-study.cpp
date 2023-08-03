#define BOOST_TEST_MODULE test-end-to-end tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

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
	TimeSeriesConfig<Matrix<double, int32_t>> loadTSconfig;
};

StudyFixture::StudyFixture()
{
	simulationBetweenDays(0, 7);
	area = addAreaToStudy("Some area");
	cluster = addClusterToArea(area, "some cluster");

	loadInArea = 7.0;
	loadTSconfig = std::move(TimeSeriesConfig<Matrix<double, int32_t>>(area->load.series->timeSeries));
	loadTSconfig.setNumberColumns(1)
				.fillColumnWith(0, loadInArea);

	clusterConfig = std::move(ThermalClusterConfig(cluster.get()));
	clusterConfig.setNominalCapacity(100.);
	clusterConfig.setAvailablePower(0, 50.);
	clusterCost = 2.;
	clusterConfig.setCosts(clusterCost);
	clusterConfig.setUnitCount(1);
};


BOOST_FIXTURE_TEST_SUITE(ONE_AREA__ONE_THERMAL_CLUSTER, StudyFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_fullfills_area_demand)
{	
	setNumberMCyears(1);

	simulation->create();
	simulation->run();

	BOOST_TEST(output->overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output->load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_MC_years__thermal_cluster_fullfills_area_demand_on_2nd_year_as_well)
{
	setNumberMCyears(2);

	simulation->create();
	playOnlyYear(1);
	simulation->run();

	BOOST_TEST(output->overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output->load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years__two_ts_identical)
{
	setNumberMCyears(2);

	loadTSconfig.setNumberColumns(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 7.0);

	clusterConfig.setAvailablePowerNumberOfTS(2);
	clusterConfig.setAvailablePower(0, 50.);
	clusterConfig.setAvailablePower(1, 50.);

	simulation->create();
	simulation->run();

	BOOST_TEST(output->overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output->load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years__two_ts_for_load)
{
	setNumberMCyears(2);

	loadTSconfig.setNumberColumns(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 14.0);

	ScenarioBuilderRule scenarioBuilderRule(*study);
	scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
	scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);

	simulation->create();
	simulation->run();

	double averageLoad = (7. + 14.) / 2.;
	BOOST_TEST(output->thermalGeneration(cluster.get()).hour(10) == averageLoad, tt::tolerance(0.001));
	BOOST_TEST(output->overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years_with_different_weight__two_ts)
{
	setNumberMCyears(2);

	giveWeightToYear(4.f, 0);
	giveWeightToYear(10.f, 1);
	float weightSum = study->parameters.getYearsWeightSum();

	loadTSconfig.setNumberColumns(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 14.0);

	ScenarioBuilderRule scenarioBuilderRule(*study);
	scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
	scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);
	
	simulation->create();
	simulation->run();
	
	double averageLoad = (4 * 7. + 10. * 14.) / weightSum;
	BOOST_TEST(output->thermalGeneration(cluster.get()).hour(10) == averageLoad, tt::tolerance(0.001));
	BOOST_TEST(output->overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()