#define BOOST_TEST_MODULE test-end-to-end tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <antares/study/study.h>

#include <simulation/simulation.h>
#include <simulation/solver.h>
#include <simulation/economy.h>

#include <iostream>
#include <stdio.h>

namespace utf = boost::unit_test;

using namespace Yuni;
using namespace Antares::Data;

BOOST_AUTO_TEST_SUITE(simple_test)

void prepareStudy(Study::Ptr pStudy, int nbYears)
{
	//TODO : define study parameters
	pStudy->parameters.reset();
	pStudy->parameters.years(nbYears);

	//Prepare parameters for simulation
	Data::StudyLoadOptions options;
	pStudy->parameters.prepareForSimulation(options);

	// Logical cores
	// -------------------------
	// Getting the number of logical cores to use before loading and creating the areas :
	// Areas need this number to be up-to-date at construction.
	pStudy->getNumberOfCores(false, 0);

	// Define as current study
	Data::Study::Current::Set(pStudy);
}

Area* addArea(Study::Ptr pStudy, const std::string& areaName, int load, int nbTS)
{
	Area* pArea = pStudy->areaAdd(areaName);

	BOOST_CHECK(pArea != NULL);

	//Need to add unsupplied energy cost constraint so load is respected
	pArea->thermal.unsuppliedEnergyCost = 10000.0;
	pArea->spreadUnsuppliedEnergyCost	= 0.01;

	//Define default load
	pArea->load.series->series.resize(nbTS, HOURS_PER_YEAR);
	pArea->load.series->series.fill(load);
	//pArea->load.series->series.multiplyColumnBy(0, 1);
	//pArea->load.series->series.multiplyColumnBy(1, 200);

	return pArea;
}

ThermalCluster* addCluster(Study::Ptr pStudy, Area* pArea, double availablePower, double cost, int nbTS)
{
	ThermalCluster* pCluster = new ThermalCluster(pArea, pStudy->maxNbYearsInParallel);
	pCluster->name("cluster");
	pCluster->reset();

	//Only one unit, nominal capacity alway power *10.0
	pCluster->unitCount = 1;
	pCluster->nominalCapacity = availablePower * 10.0;

	//Power cost
	pCluster->marginalCost	= cost;

	//TODO JMK Must define merket bid cost otherwise all production is used
	pCluster->marketBidCost = cost;

	//Must define  min stable power always 0.0
	pCluster->minStablePower = 0.0;

	//Define power consumption
	pCluster->series->series.resize(nbTS, HOURS_PER_YEAR);
	pCluster->series->series.fill(availablePower);

	//No modulation on cost
	pCluster->modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
	pCluster->modulation.fill(1.);
	pCluster->modulation.fillColumn(thermalMinGenModulation, 0.);

	//Initialize production cost from modulation
	if (not pCluster->productionCost)
		pCluster->productionCost = new double[HOURS_PER_YEAR];

	
	double* prodCost	= pCluster->productionCost;
	double marginalCost = pCluster->marginalCost;

	// Production cost
	auto& modulation = pCluster->modulation[thermalModulationCost];
	for (uint h = 0; h != pCluster->modulation.height; ++h)
		prodCost[h] = marginalCost * modulation[h];

	
	pCluster->nominalCapacityWithSpinning = pCluster->nominalCapacity;

	BOOST_CHECK(pArea->thermal.list.add(pCluster));

	pArea->thermal.list.mapping[pCluster->id()] = pCluster;

	return pCluster;
}


//Very simple test with one area and one load and one year
BOOST_AUTO_TEST_CASE(very_simple_test)
{
	//Create study
	Study::Ptr pStudy = new Study(true /* for the solver */);

	//TODO JMK :for now only tested with one year and one TS
	int nbYears = 1;
	int  nbTS	= 1;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad		= nbTS;
	pStudy->parameters.nbTimeSeriesThermal	= nbTS;

	//Create area
	int load = 7;
	Area*  pArea = addArea(pStudy,"Area 1",load, nbTS);	

	//Add thermal  cluster
	double availablePower	= 10.0;
	double cost				= 2.0;
	ThermalCluster* pCluster = addCluster(pStudy, pArea, availablePower,cost, nbTS);

	// Runtime data dedicated for the solver
	BOOST_CHECK(pStudy->initializeRuntimeInfos());	

	Settings pSettings;
	pSettings.tsGeneratorsOnly = false;
	pSettings.noOutput		   = false;

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = new Solver::Simulation::ISimulation< Solver::Simulation::Economy >(*pStudy, pSettings);

	// Allocate all arrays
	SIM_AllocationTableaux();

	// Let's go
	simulation->run();

	// Define variable tested
	typedef Solver::Variable::Economy::VCardOverallCost OverallCostVCard;
	typedef Solver::Variable::Storage<OverallCostVCard>::ResultsType  OverallCostResults;

	OverallCostResults* overallCost = nullptr;
	simulation->variables.retrieveResultsForArea<OverallCostVCard>(&overallCost, pArea);

	//TODO JMK : allYears is not calculated, need to call SurveyResults (to be checked)
	BOOST_CHECK(overallCost->avgdata.nbYearsCapacity == nbYears);

	BOOST_CHECK(overallCost->avgdata.hourly[0]	== load * cost);
	BOOST_CHECK(overallCost->avgdata.daily[0]	== load * cost * 24);
	BOOST_CHECK(overallCost->avgdata.weekly[0]	== load * cost * 24 * 7);

	//TODO JMK : monthly and year values depends on nbDays in month and nbDays in year
	//To be checked after
	//BOOST_CHECK(overallCost->avgdata.monthly[0] == power * 24 * 31);
	//BOOST_CHECK(overallCost->avgdata.year[0] == power * 24 * 365);	


	// Define variable tested
	typedef Solver::Variable::Economy::VCardTimeSeriesValuesLoad TimeSeriesValuesLoadVCard;
	typedef Solver::Variable::Storage<TimeSeriesValuesLoadVCard>::ResultsType  TimeSeriesValuesLoadResults;

	TimeSeriesValuesLoadResults* timeSeriesLoad = nullptr;
	simulation->variables.retrieveResultsForArea<TimeSeriesValuesLoadVCard>(&timeSeriesLoad, pArea);

	//TODO JMK : allYears is not calculated, need to call SurveyResults (to be checked)
	BOOST_CHECK(timeSeriesLoad->avgdata.nbYearsCapacity == nbYears);

	BOOST_CHECK(timeSeriesLoad->avgdata.hourly[0] == load);
	BOOST_CHECK(timeSeriesLoad->avgdata.daily[0] == load * 24);
	BOOST_CHECK(timeSeriesLoad->avgdata.weekly[0] == load * 24 * 7);

	//TODO JMK : monthly and year values depends on nbDays in month and nbDays in year
	//To be checked after
	//BOOST_CHECK(timeSeriesLoad->avgdata.monthly[0] == load * 24 * 31);
	//BOOST_CHECK(timeSeriesLoad->avgdata.year[0] == load * 24 * 365);	

	// simulation
	SIM_DesallocationTableaux();

	delete simulation;
	
	// release all reference to the current study held by this class
	pStudy->clear();

	pStudy = nullptr;
	// removed any global reference
	Data::Study::Current::Set(nullptr);
}

BOOST_AUTO_TEST_SUITE_END()