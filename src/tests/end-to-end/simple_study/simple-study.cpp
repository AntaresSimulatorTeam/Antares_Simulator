/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#define BOOST_TEST_MODULE test-end-to-end tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <antares/writer/in_memory_writer.h>

#include "utils.h"
//#include <fstream>

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

    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea,
               tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 1, tt::tolerance(0.001));
    BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(milp_two_mc_two_unit_single_scenario)
{
    setNumberMCyears(1);

    clusterConfig.setAvailablePower(0, 150.).setUnitCount(2);

    loadInArea = 150;
    loadTSconfig.setColumnCount(1).fillColumnWith(0, loadInArea);
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

    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == loadInArea,
               tt::tolerance(0.001));
    BOOST_TEST(output.thermalNbUnitsON(cluster.get()).hour(10) == 2, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(parallel)
{
	setNumberMCyears(10);
    study->maxNbYearsInParallel = 2;

	simulation->create();
	simulation->run();

	OutputRetriever output(simulation->rawSimu());
	BOOST_TEST(output.overallCost(area).hour(0) == loadInArea * clusterCost, tt::tolerance(0.001));
	BOOST_TEST(output.load(area).hour(0) == loadInArea, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(parallel2)
{
	setNumberMCyears(2);
    study->maxNbYearsInParallel = 2;

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

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(error_cases)
BOOST_AUTO_TEST_CASE(error_on_wrong_hydro_data)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 7);
    builder.setNumberMCyears(1);
    Area& area = *builder.addAreaToStudy("A");
    PartHydro& hydro = area.hydro;
    TimeSeriesConfigurer(hydro.series->storage.timeSeries)
            .setColumnCount(1)
            .fillColumnWith(0, -1.0); //Negative inflow will cause a consistency error with mingen

    auto simulation = builder.simulation;
    simulation->create();
    BOOST_CHECK_THROW(simulation->run(), Antares::FatalError);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(ONE_AREA__ONE_STS_THERMAL_CLUSTER, StudyFixture)
BOOST_AUTO_TEST_CASE(sts_initial_level)
{
    using namespace Antares::Data::ShortTermStorage;
	setNumberMCyears(1);
    auto& storages = area->shortTermStorage.storagesByIndex;
    STStorageCluster sts;
    auto& props = sts.properties;
    props.name = "my-sts";
    props.injectionNominalCapacity = 10;
    props.withdrawalNominalCapacity = 10;
    props.reservoirCapacity = 100;
    props.efficiencyFactor = .9;
    props.initialLevel = .443;
    props.group = Group::PSP_open;
    // Default values for series
    sts.series->fillDefaultSeriesIfEmpty();

    storages.push_back(sts);
    auto& hydro = area->hydro;
    hydro.reservoirCapacity = 1.e6;
    hydro.reservoirManagement = true;
    hydro.useHeuristicTarget = true;
    hydro.useLeeway = false;
    hydro.useWaterValue = false;
    hydro.pumpingEfficiency = .9;

    // Fatal gen at h=1
    {
      auto& windTS = area->wind.series.timeSeries;
      TimeSeriesConfigurer(windTS)
        .setColumnCount(1)
        .fillColumnWith(0, 0.);
      windTS[0][1] = 100;
    }

    // Fatal load at h=2
    {
      auto& loadTS = area->load.series.timeSeries;
      TimeSeriesConfigurer(loadTS)
        .setColumnCount(1)
        .fillColumnWith(0, 0.);
      loadTS[0][2] = 100;
    }

    const double dailyInflow_MWh = 0.;
    // Inflow = 100MW for every day
    {
      auto& inflowTS = hydro.series->storage.timeSeries;
      TimeSeriesConfigurer(inflowTS)
        .setColumnCount(1, DAYS_PER_YEAR)
        .fillColumnWith(0, dailyInflow_MWh);
    }

    {
      auto& maxPower = hydro.maxPower;
      TimeSeriesConfigurer(maxPower)
        .setColumnCount(4, DAYS_PER_YEAR)
        .fillColumnWith(hydro.genMaxE, 24.)
        .fillColumnWith(hydro.genMaxP, 100.)
        .fillColumnWith(hydro.pumpMaxE, 24.)
        .fillColumnWith(hydro.pumpMaxP, 50.);
    }

    // Usual values, avoid spillage & unsupplied energy
    area->thermal.unsuppliedEnergyCost = 1.e3;
    area->thermal.spilledEnergyCost = 1.;

    study->parameters.noOutput = true;
    study->parameters.namedProblems = true;
    study->parameters.include.exportMPS = Data::mpsExportStatus::EXPORT_BOTH_OPTIMS;



    NullDurationCollector nullDurationCollector;
    Antares::Solver::InMemoryWriter writer(nullDurationCollector);
	simulation->create(writer);
	simulation->run();

    const auto& inMemoryResult = writer.getMap();

    // {
    //     const std::string m2 = "problem-1-1--optim-nb-1.mps";
    //     auto& mps = inMemoryResult.at(m2);
    //     std::ofstream os("/tmp/a.mps");
    //     os << mps;
    // }

    std::string str;
    std::vector<double> turbCreditWeek;
    const std::string magic = "debug/solver/1/week-0interval-0_some area.txt";
    std::istringstream is(inMemoryResult.at(magic));
    while(getline(is, str))
    {
        turbCreditWeek.push_back(std::stod(str));
    }
    OutputRetriever output(simulation->rawSimu());

    double deltaLevel_MWh = hydro.reservoirCapacity * (output.hydroLevel(area).hour(167) - output.hydroLevel(area).hour(0)) / 100; // This is not the real level difference, since we'd need hour -1 that doesn't exist, but it's pretty close
    double turbCreditReal_MWh = turbCreditWeek[0]; // In 'weekly' simulations, only the 1st element (out of 7) is actually used

    double sumInflows_MWh = 7 * dailyInflow_MWh;
    // TODO Why does this fail ?
    BOOST_TEST(deltaLevel_MWh + turbCreditReal_MWh == sumInflows_MWh, tt::tolerance(.01));
}

BOOST_AUTO_TEST_SUITE_END()
