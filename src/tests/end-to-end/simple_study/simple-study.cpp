#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
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
	TimeSeriesConfigurer<Matrix<double, int32_t>> loadTSconfig;
};

StudyFixture::StudyFixture()
{
	simulationBetweenDays(0, 7);
	area = addAreaToStudy("Some area");
	cluster = addClusterToArea(area, "some cluster");

	loadInArea = 7.0;
	loadTSconfig = std::move(TimeSeriesConfigurer(area->load.series->timeSeries));
	loadTSconfig.setColumnCount(1)
				.fillColumnWith(0, loadInArea);

	clusterCost = 2.;
	clusterConfig = std::move(ThermalClusterConfig(cluster.get()));
	clusterConfig.setNominalCapacity(100.)
				 .setAvailablePower(0, 50.)
				 .setCosts(clusterCost)
				 .setUnitCount(1);
};


BOOST_FIXTURE_TEST_SUITE(ONE_AREA__ONE_THERMAL_CLUSTER, StudyFixture)

BOOST_AUTO_TEST_CASE(thermal_cluster_fullfills_area_demand)
{	
	setNumberMCyears(1);

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_MC_years__thermal_cluster_fullfills_area_demand_on_2nd_year_as_well)
{
	setNumberMCyears(2);

	simulation->create();
	playOnlyYear(1);
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years__two_ts_identical)
{
	setNumberMCyears(2);

	loadTSconfig.setColumnCount(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 7.0);

	clusterConfig.setAvailablePowerNumberOfTS(2)
				 .setAvailablePower(0, 50.)
				 .setAvailablePower(1, 50.);

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years__two_ts_for_load)
{
	setNumberMCyears(2);

	loadTSconfig.setColumnCount(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 14.0);

	ScenarioBuilderRule scenarioBuilderRule(*study);
	scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
	scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	double averageLoad = (7. + 14.) / 2.;
	BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == averageLoad, tt::tolerance(0.001));
	BOOST_TEST(output.overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(two_mc_years_with_different_weight__two_ts)
{
	setNumberMCyears(2);

	giveWeightToYear(4.f, 0);
	giveWeightToYear(10.f, 1);
	float weightSum = study->parameters.getYearsWeightSum();

	loadTSconfig.setColumnCount(2)
				.fillColumnWith(0, 7.0)
				.fillColumnWith(1, 14.0);

	ScenarioBuilderRule scenarioBuilderRule(*study);
	scenarioBuilderRule.load().setTSnumber(area->index, 0, 1);
	scenarioBuilderRule.load().setTSnumber(area->index, 1, 2);
	
	simulation->create();
	simulation->run();
	
	OutputRetriever output(simulation->rawSimu());
	double averageLoad = (4 * 7. + 10. * 14.) / weightSum;
	BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == averageLoad, tt::tolerance(0.001));
	BOOST_TEST(output.overallCost(area).hour(0) == averageLoad * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(milp_two_mc_single_unit_single_scenario)
{
	setNumberMCyears(1);

    // Arbitrary large number, only characteristic is : larger than all
    // other marginal costs
    area->thermal.unsuppliedEnergyCost = 1000;

    // Use OR-Tools / COIN for MILP
    auto& p = study->parameters;
    p.unitCommitment.ucMode = ucMILP;
    p.ortoolsUsed = true;
    p.ortoolsSolver = "coin";

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());

	BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea, tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 1, tt::tolerance(0.001));
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(milp_two_mc_two_unit_single_scenario)
{
	setNumberMCyears(1);

	clusterConfig.setAvailablePower(0, 150.)
				 .setUnitCount(2);

	loadInArea = 150;
	loadTSconfig.setColumnCount(1)
				.fillColumnWith(0, loadInArea);
    // Arbitrary large number, only characteristic is : larger than all
    // other marginal costs
    area->thermal.unsuppliedEnergyCost = 1000;

    // Use OR-Tools / COIN for MILP
    auto& p = study->parameters;
    p.unitCommitment.ucMode = ucMILP;
    p.ortoolsUsed = true;
    p.ortoolsSolver = "coin";

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());

	BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea, tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 2, tt::tolerance(0.001));

}

BOOST_AUTO_TEST_SUITE_END()
