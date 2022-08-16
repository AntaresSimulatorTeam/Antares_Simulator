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
namespace tt = boost::test_tools;

using namespace Yuni;
using namespace Antares::Data;

BOOST_AUTO_TEST_SUITE(simple_test)

// checkVariable<VCard>(simulation, pArea, expectedHourlyVal)
//
// Check variable value from VCard
// Template param :
//		VCard				: VCard defining variable (Solver::Variable::Economy::VCardOverallCost for example)
// classical params :
//		simulation 			: Simulation object containing results
//		area				: Area to be checked
//		expectedHourlyValue	: Expected hourly value
template<class VCard>
void checkVariable(
						Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation,
						Area* pArea,
						double expectedHourlyValue
				   )

{ 
	/*Get value*/
	typename Antares::Solver::Variable::Storage<VCard>::ResultsType* result = nullptr;
	simulation->variables.retrieveResultsForArea<VCard>(&result, pArea);
	BOOST_TEST(result->avgdata.hourly[0] == expectedHourlyValue,			tt::tolerance(0.001));
	BOOST_TEST(result->avgdata.daily[0]  == expectedHourlyValue * 24,		tt::tolerance(0.001));
	BOOST_TEST(result->avgdata.weekly[0] == expectedHourlyValue * 24 * 7,	tt::tolerance(0.001));
}

void prepareStudy(Study::Ptr pStudy, int nbYears)
{
	//Define study parameters
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

Area* addArea(Study::Ptr pStudy, const std::string& areaName, int nbTS)
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


std::shared_ptr<ThermalCluster> addCluster(Study::Ptr pStudy, Area* pArea, const std::string& clusterName, double maximumPower, double cost, int nbTS, int unitCount = 1)
{
    auto pCluster = std::make_shared<ThermalCluster>(pArea, pStudy->maxNbYearsInParallel);
	pCluster->setName(clusterName);
	pCluster->reset();
	
	pCluster->unitCount			= unitCount;
	pCluster->nominalCapacity	= maximumPower;

	//Power cost
	pCluster->marginalCost	= cost;

	//Must define market bid cost otherwise all production is used
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

    auto added = pArea->thermal.list.add(pCluster);

	BOOST_CHECK(added != nullptr);

    pArea->thermal.list.mapping[pCluster->id()] = added;

	return pCluster;
}

ScenarioBuilder::Rules::Ptr createScenarioRules(Study::Ptr pStudy)
{
	ScenarioBuilder::Rules::Ptr pRules;

	pStudy->scenarioRulesCreate();
	ScenarioBuilder::Sets* p_sets = pStudy->scenarioRules;
	if (p_sets && !p_sets->empty())
	{
		pRules = p_sets->createNew("Custom");

		pStudy->parameters.useCustomScenario  = true;
		pStudy->parameters.activeRulesScenario = "Custom";
	}

	return pRules;
}

float defineYearsWeight(Study::Ptr pStudy, const std::vector<float>& yearsWeight)
{
	pStudy->parameters.userPlaylist = true;

	for (uint i = 0; i < yearsWeight.size(); i++)
	{
		pStudy->parameters.setYearWeight(i, yearsWeight[i]);
	}
    
	return pStudy->parameters.getYearsWeightSum();
}

Solver::Simulation::ISimulation< Solver::Simulation::Economy >* runSimulation(Study::Ptr pStudy)
{
	// Runtime data dedicated for the solver
	BOOST_CHECK(pStudy->initializeRuntimeInfos());

	Settings pSettings;
	pSettings.tsGeneratorsOnly = false;
	pSettings.noOutput = false;

	//Launch simulation
	Benchmarking::NullDurationCollector nullDurationCollector;
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = new Solver::Simulation::ISimulation< Solver::Simulation::Economy >(*pStudy, pSettings, &nullDurationCollector);

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

//Very simple test with one area and one load and one year
BOOST_AUTO_TEST_CASE(one_mc_year_one_ts)
{
	//Create study
	Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver

	//On year  and one TS
	int nbYears = 1;
	int  nbTS	= 1;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad		= nbTS;
	pStudy->parameters.nbTimeSeriesThermal	= nbTS;

	//Create area
	double load = 7.0;
	Area*  pArea = addArea(pStudy,"Area 1", nbTS);	

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);

	//Add thermal  cluster
	double availablePower	= 50.0;
	double cost				= 2.0;
	double maximumPower		= 100.0;
	auto pCluster = addCluster(pStudy, pArea,"Cluster 1", maximumPower,cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);
		
	//Overall cost must be load * cost by MW
	checkVariable<Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable<Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);	
}

//Very simple test with one area and one load and two year
BOOST_AUTO_TEST_CASE(two_mc_year_one_ts)
{
	//Create study
	Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver

	//On year  and one TS
	int nbYears = 2;
	int  nbTS	= 1;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	double load = 7.0;
	Area* pArea = addArea(pStudy, "Area 1", nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);

	//Add thermal  cluster
	double availablePower	= 10.0;
	double cost				= 2.0;
	double maximumPower		= 100.0;
	auto pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable<Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable<Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}


//Very simple test with one area and one load and two year and two identical TS
BOOST_AUTO_TEST_CASE(two_mc_year_two_ts_identical)
{
	//Create study
	Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver

	//On year  and one TS
	int nbYears = 2;
	int  nbTS	= 2;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	double load = 7.0;
	Area* pArea = addArea(pStudy, "Area 1", nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);
	pArea->load.series->series.fillColumn(1, load);

	//Add thermal  cluster
	double availablePower = 10.0;
	double cost = 2.0;
	double maximumPower = 100.0;
	auto pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);
	pCluster->series->series.fillColumn(1, availablePower);

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable<Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, load * cost);

	//Load must be load
	checkVariable<Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, load);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}



//Very simple test with one area and one load and two year and two TS with different load
BOOST_AUTO_TEST_CASE(two_mc_year_two_ts)
{
	//Create study
	Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver

	//On year  and one TS
	int nbYears = 2;
	int  nbTS = 2;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

	//Create area
	double load = 5.0;
	Area* pArea = addArea(pStudy, "Area 1", nbTS);

	//Initialize time series
	pArea->load.series->series.fillColumn(0, load);
	pArea->load.series->series.fillColumn(1, load * 2);

	double averageLoad = load * 1.5;

	//Add thermal  cluster
	double availablePower	= 20.0;
	double cost				= 2.2;
	double maximumPower		= 100.0;
	auto pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);
	pCluster->series->series.fillColumn(1, availablePower);

	//Create scenario rules to force use of TS otherwise the TS used is random
	std::vector<int> areaLoadTS;
	areaLoadTS.assign(nbYears, 1);
	areaLoadTS[0] = 1;	areaLoadTS[1] = 2;

	ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(pStudy);
	for (int i = 0; i < nbYears; i++)
	{
		pRules->load.set(pArea->index, i, areaLoadTS[i]);
	}

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable<Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, averageLoad * cost);

	//Load must be load
	checkVariable<Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, averageLoad);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}



//Very simple test with one area and one load and two year with different load and weight
BOOST_AUTO_TEST_CASE(two_mc_year_two_ts_different_weight)
{
	//Create study
	Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver

	//Two years  and two TS
	int nbYears = 2;
	int  nbTS	= 2;

	//Prepare study
	prepareStudy(pStudy, nbYears);
	pStudy->parameters.nbTimeSeriesLoad = nbTS;
	pStudy->parameters.nbTimeSeriesThermal = nbTS;

    //Define years weight
	std::vector<float> yearsWeight;
	yearsWeight.assign(nbYears, 1);
	yearsWeight[0] = 4.f;	yearsWeight[1] = 10.f;

	float yearSum = defineYearsWeight(pStudy,yearsWeight);

	//Create area
	double load = 5.0;
	Area* pArea = addArea(pStudy, "Area 1", nbTS);

	//Initialize time series
	std::vector<double> loadList;
	loadList.assign(nbTS, load);
	loadList[1] =  load * 2;

	for (int i = 0; i < nbTS; i++)
	{
		pArea->load.series->series.fillColumn(i, loadList[i]);
	}	

	//Add thermal  cluster
	double availablePower	= 20.0;
	double cost				= 2.2;
	double maximumPower		= 100.0;
	auto pCluster = addCluster(pStudy, pArea, "Cluster 1", maximumPower, cost, nbTS);

	//Initialize time series
	pCluster->series->series.fillColumn(0, availablePower);
	pCluster->series->series.fillColumn(1, availablePower);

	//Create scenario rules to force use of TS otherwise the TS used is random
	std::vector<int> areaLoadTS;
	areaLoadTS.assign(nbYears, 1);
	areaLoadTS[0] = 1;	areaLoadTS[1] = 2;

	ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(pStudy);
	for (int i = 0; i < nbYears; i++)
	{
		pRules->load.set(pArea->index, i, areaLoadTS[i]);
	}

	//Calculate average load with mc years weight
	double averageLoad = 0.0;
	for (int i = 0; i < nbYears; i++)
	{
		averageLoad += loadList[areaLoadTS[i] - 1 ] * yearsWeight[i] / yearSum;
	}

	//Launch simulation
	Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

	//Overall cost must be load * cost by MW
	checkVariable<Solver::Variable::Economy::VCardOverallCost>(simulation, pArea, averageLoad * cost);

	//Load must be load
	checkVariable<Solver::Variable::Economy::VCardTimeSeriesValuesLoad>(simulation, pArea, averageLoad);

	//Clean simulation
	cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_SUITE_END()
