#define BOOST_TEST_MODULE test-end-to-end tests

#define WIN32_LEAN_AND_MEAN

#include <boost/test/included/unit_test.hpp>

#include <antares/study/study.h>
#include <antares/study/scenario-builder/sets.h>

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
	pArea->load.series->series.fill(0.0);

	return pArea;
}


ThermalCluster* addCluster(Study::Ptr pStudy, Area* pArea, const std::string& clusterName, double maximumPower, double cost, int nbTS, int unitCount = 1)
{
	ThermalCluster* pCluster = new ThermalCluster(pArea, pStudy->maxNbYearsInParallel);
	pCluster->name(clusterName);
	pCluster->reset();
	
	pCluster->unitCount			= unitCount;
	pCluster->nominalCapacity	= maximumPower;

	//Power cost
	pCluster->marginalCost	= cost;

	//TODO JMK Must define merket bid cost otherwise all production is used
	pCluster->marketBidCost = cost;

	//Must define  min stable power always 0.0
	pCluster->minStablePower = 0.0;

	//Define power consumption
	pCluster->series->series.resize(nbTS, HOURS_PER_YEAR);
	pCluster->series->series.fill(0.0);

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


Solver::Simulation::ISimulation< Solver::Simulation::Economy >* runSimulation(Study::Ptr pStudy)
{
	// Runtime data dedicated for the solver
	BOOST_CHECK(pStudy->initializeRuntimeInfos());

	Settings pSettings;
	pSettings.tsGeneratorsOnly = false;
	pSettings.noOutput = false;

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = new Solver::Simulation::ISimulation< Solver::Simulation::Economy >(*pStudy, pSettings);

	// Allocate all arrays
	SIM_AllocationTableaux();

	// Let's go
	simulation->run();

	return simulation;
}

void cleanSimulation(Study::Ptr pStudy, Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation)
{
	// simulation
	SIM_DesallocationTableaux();

	delete simulation;

	// release all reference to the current study held by this class
	pStudy->clear();

	pStudy = nullptr;
	// removed any global reference
	Data::Study::Current::Set(nullptr);
}

template<typename VCard,class T>
void checkVariable(Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation, Area* pArea,
				   T expectedHourlyValue)
{
	//Get value
	Solver::Variable::Storage<VCard>::ResultsType* result = nullptr;
	simulation->variables.retrieveResultsForArea<VCard>(&result, pArea);

	//TODO JMK : allYears is not calculated, need to call SurveyResults (to be checked)
	BOOST_CHECK(result->avgdata.hourly[0] == expectedHourlyValue);
	BOOST_CHECK(result->avgdata.daily[0]  == expectedHourlyValue * 24);
	BOOST_CHECK(result->avgdata.weekly[0] == expectedHourlyValue * 24 * 7);

	//TODO JMK : monthly and year values depends on nbDays in month and nbDays in year
	//To be checked after
	//BOOST_CHECK(overallCost->avgdata.monthly[0] == power * 24 * 31);
	//BOOST_CHECK(overallCost->avgdata.year[0] == power * 24 * 365);
}

//Very simple test with one area and one load and one year
BOOST_AUTO_TEST_CASE(one_mc_year_one_ts)
{
	//Create study
	Study::Ptr pStudy = new Study(true /* for the solver */);

	//On year  and one TS
	int nbYears = 1;
	int  nbTS	= 1;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad		= nbTS;
	pStudy->parameters.nbTimeSeriesThermal	= nbTS;

	//Create area
	int load = 7;
	Area*  pArea = addArea(pStudy,"Area 1",load, nbTS);	

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);

	//Add thermal  cluster
	double availablePower	= 50.0;
	double cost				= 2.0;
	double maximumPower		= 100.0;
	ThermalCluster* pCluster = addCluster(pStudy, pArea,"Cluster 1", maximumPower,cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);
		
	//Overall cost must be load * cost by MW
	checkVariable< Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable< Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);	
}

//Very simple test with one area and one load and two year
BOOST_AUTO_TEST_CASE(two_mc_year_one_ts)
{
	//Create study
	Study::Ptr pStudy = new Study(true /* for the solver */);

	//On year  and one TS
	int nbYears = 2;
	int  nbTS	= 1;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	int load = 7;
	Area* pArea = addArea(pStudy, "Area 1", load, nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);

	//Add thermal  cluster
	double availablePower	= 10.0;
	double cost				= 2.0;
	double maximumPower		= 100.0;
	ThermalCluster* pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable< Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable< Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}


//Very simple test with one area and one load and two year and two identical TS
BOOST_AUTO_TEST_CASE(two_mc_year_two_ts_identical)
{
	//Create study
	Study::Ptr pStudy = new Study(true /* for the solver */);

	//On year  and one TS
	int nbYears = 2;
	int  nbTS	= 2;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	int load = 7;
	Area* pArea = addArea(pStudy, "Area 1", load, nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);
	pArea->load.series->series.fillColumn(1, load);

	//Add thermal  cluster
	double availablePower = 10.0;
	double cost = 2.0;
	double maximumPower = 100.0;
	ThermalCluster* pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);
	pCluster->series->series.fillColumn(1, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable< Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable< Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}

//Very simple test with one area and one load and two year and two TS with different load
BOOST_AUTO_TEST_CASE(two_mc_year_two_ts)
{
	//Create study
	Study::Ptr pStudy = new Study(true /* for the solver */);

	//On year  and one TS
	int nbYears = 2;
	int  nbTS = 2;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	int load = 5;
	Area* pArea = addArea(pStudy, "Area 1", load, nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);
	pArea->load.series->series.fillColumn(1, load * 2 );

	double averageLoad = load * 1.5;

	//Add thermal  cluster
	double availablePower	= 20.0;
	double cost				= 1.0;
	double maximumPower		= 100.0;
	ThermalCluster* pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);
	pCluster->series->series.fillColumn(1, availablePower);

	//Create scenario rules to force use of TS otherwise the TS used is random
	pStudy->scenarioRulesCreate();
	ScenarioBuilder::Sets* p_sets = pStudy->scenarioRules;
	
	
	if (p_sets && !p_sets->empty())
	{
		ScenarioBuilder::Rules::Ptr pRules = p_sets->createNew("Custom");
		pRules->load.set(pArea->index, 1, 1);
		pRules->load.set(pArea->index, 2, 2);

		pStudy->parameters.useCustomTSNumbers = true;
		pStudy->parameters.activeRulesScenario = "Custom";
	}


	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	//TODO JMK : for now we can check with several disctinct MC years because we can't use ScenarioBuilder (always using study parameters from file : not available with end to end test which are done in memory)
	//checkVariable< Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, averageLoad * cost);

	//Load must be load
	//TODO JMK : for now we can check with several disctinct MC years because we can't use ScenarioBuilder (always using study parameters from file : not available with end to end test which are done in memory)
	//checkVariable< Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, averageLoad);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_SUITE_END()