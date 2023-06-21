#define BOOST_TEST_MODULE test-end-to-end tests_binding_constraints
#define WIN32_LEAN_AND_MEAN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "utils.h"
#include "simulation.h"

#include "antares/study/study.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

using namespace Antares::Data;
using namespace Antares::Solver;
using namespace Antares::Solver::Simulation;
using namespace Benchmarking;

// ====================
// Helper functions
// ====================

void initializeStudy(Study::Ptr study)
{
    study->resultWriter = std::make_shared<NoOPResultWriter>();
    study->parameters.reset();
    study->maxNbYearsInParallel = 1;
    Data::Study::Current::Set(study);
}


Area* addAreaToStudy(Study::Ptr study, const std::string& areaName, double loadInArea)
{
    Area* area = study->areaAdd(areaName);

    BOOST_CHECK(area != NULL);

    area->thermal.unsuppliedEnergyCost = 1000.0;
    area->spreadUnsuppliedEnergyCost	= 0.;

    //Define default load
    unsigned int loadNumberTS = 1;
    area->load.series->timeSeries.resize(loadNumberTS, HOURS_PER_YEAR);
    area->load.series->timeSeries.fill(loadInArea);

    return area;
}

void configureLinkCapacities(AreaLink* link)
{
    double linkCapacityInfinite = std::numeric_limits<double>::infinity();
    link->directCapacities.resize(1, 8760);
    link->directCapacities.fill(linkCapacityInfinite);

    link->indirectCapacities.resize(1, 8760);
    link->indirectCapacities.fill(linkCapacityInfinite);
}

std::shared_ptr<ThermalCluster> addClusterToArea(Area* area, const std::string& clusterName)
{
    auto cluster = std::make_shared<ThermalCluster>(area);
    cluster->setName(clusterName);
    cluster->reset();

    double availablePower = 100.0;
    double maximumPower = 100.0;
    double clusterCost = 50.;
    unsigned int nbTS = 1;
    unsigned int unitCount = 1;

    cluster->unitCount			= unitCount;
    cluster->nominalCapacity	= maximumPower;

    //Power cost
    cluster->marginalCost	= clusterCost;

    //Must define market bid cost otherwise all production is used
    cluster->marketBidCost = clusterCost;

    //Must define  min stable power always 0.0
    cluster->minStablePower = 0.0;

    //Define power consumption
    cluster->series->timeSeries.resize(nbTS, HOURS_PER_YEAR);
    cluster->series->timeSeries.fill(availablePower);

    //No modulation on cost
    cluster->modulation.reset(thermalModulationMax, HOURS_PER_YEAR);
    cluster->modulation.fill(1.);
    cluster->modulation.fillColumn(thermalMinGenModulation, 0.);

    //Initialize production cost from modulation
    if (not cluster->productionCost)
        cluster->productionCost = new double[HOURS_PER_YEAR];


    double* prodCost	= cluster->productionCost;
    double marginalCost = cluster->marginalCost;

    // Production cost
    auto& modulation = cluster->modulation[thermalModulationCost];
    for (uint h = 0; h != cluster->modulation.height; ++h)
        prodCost[h] = marginalCost * modulation[h];


    cluster->nominalCapacityWithSpinning = cluster->nominalCapacity;

    auto added = area->thermal.list.add(cluster);

    BOOST_CHECK(added != nullptr);

    area->thermal.list.mapping[cluster->id()] = added;

    return cluster;
}

void addScratchpadToEachArea(Study::Ptr study)
{
    for (auto [_, area] : study->areas) {
        for (unsigned int i = 0; i < study->maxNbYearsInParallel; ++i) {
            area->scratchpad.push_back(AreaScratchpad(*study->runtime, *area));
        }
    }
}

void configureBCgroupTSnumbers(Study::Ptr study, 
                               std::string BCgroup,
                               unsigned int nbYears, 
                               unsigned int tsNumber)
{
    study->bindingConstraints.resizeAllTimeseriesNumbers(nbYears);
    auto& ts_numbers_matrix = study->bindingConstraints.groupToTimeSeriesNumbers[BCgroup];
    ts_numbers_matrix.timeseriesNumbers.fill(tsNumber);
}

void configureBCrhs(std::shared_ptr<BindingConstraint>& BC, double rhsValue)
{
    BC->RHSTimeSeries().resize(1, 8760);
    BC->RHSTimeSeries().fill(rhsValue);
}

void whenCleaningSimulation()
{
    SIM_DesallocationTableaux();
}



// ===============
// The fixture
// ===============
struct Fixture {
    Fixture();
    void runSimulation();
    ~Fixture();

    // Data members
    AreaLink* link = nullptr;
    std::shared_ptr<BindingConstraint> BC;
    std::shared_ptr<ISimulation<Economy>> simulation;
    std::shared_ptr<Study> study;
};

// ================================
// The fixture's member functions
// ================================

Fixture::Fixture()
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;

    study = std::make_shared<Study>();

    initializeStudy(study);

    double loadInAreaOne = 0.;
    Area* area1 = addAreaToStudy(study, "Area 1", loadInAreaOne);

    double loadInAreaTwo = 100.;
    Area* area2 = addAreaToStudy(study, "Area 2", loadInAreaTwo);

    link = AreaAddLinkBetweenAreas(area1, area2);

    configureLinkCapacities(link);

    addClusterToArea(area1, "some cluster");

    BC = addBindingConstraints(study, "BC1", "Group1");
    BC->weight(link, 1);
    BC->enabled(true);
};

Fixture::~Fixture()
{
    whenCleaningSimulation();
}

void  Fixture::runSimulation()
{
    // Runtime infos and scratchpad are MANDATORY for the simulation NOT TO CRASH.
    BOOST_CHECK(study->initializeRuntimeInfos());
    addScratchpadToEachArea(study);

    NullDurationCollector nullDurationCollector;
    simulation = std::make_shared<ISimulation<Economy>>(*study,
                                                        Settings(),
                                                        &nullDurationCollector);
    // Allocate arrays for time series
    SIM_AllocationTableaux();

    simulation->run();
}

double getLinkFlowAthour(std::shared_ptr<ISimulation<Economy>> simulation, AreaLink* link, unsigned int hour)
{
    // There is a problem here : 
    //    we cannot easly retrieve the hourly flow for a link and a year : 
    //    - Functions retrieveHourlyResultsForCurrentYear are not coded everywhere it should.
    //    - Even if those functions were correctly implemented, there is another problem :
    //      Each year results erases results of previous year, how can we retrieve results of year 1
    //      if 2 year were run ?
    //    We should be able to run each year independently, which is not possible now.
    //    A workaround is to retrieve syntheses, and that's what we do here.

    typename Variable::Storage<Variable::Economy::VCardFlowLinear>::ResultsType* result = nullptr;
    simulation->variables.retrieveResultsForLink<Variable::Economy::VCardFlowLinear>(&result, link);
    return result->avgdata.hourly[hour];
}


BOOST_FIXTURE_TEST_SUITE(tests_end2end_binding_constraints, Fixture)

BOOST_AUTO_TEST_CASE(BC_restricts_link_direct_capacity_to_90)
{
    // Study parameters varying depending on the test 
    unsigned int nbYears = 1;
    study->parameters.resetPlaylist(nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int sameTSnumberForEachYear = 0;
    configureBCgroupTSnumbers(study, BC->group(), nbYears, sameTSnumberForEachYear);
    
    double rhsValue = 90.;
    configureBCrhs(BC, rhsValue);
    
    runSimulation();

    unsigned int hour = 0;
    BOOST_TEST(getLinkFlowAthour(simulation, link, hour) == rhsValue, tt::tolerance(0.001));
}

//BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsWeekly)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto [_ ,link] = prepare(study, rhs, BindingConstraint::typeWeekly, BindingConstraint::opEquality);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsDaily)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto [_ ,link] = prepare(study, rhs, BindingConstraint::typeDaily, BindingConstraint::opEquality);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.daily[0] == rhs * cost, tt::tolerance(0.001));
//    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_Constraints_HourlyLess)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto [_ ,link] = prepare(study, rhs, BindingConstraint::typeHourly, BindingConstraint::opLess);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.hourly[0] < rhs * cost);
//    BOOST_TEST(result->avgdata.daily[0] < rhs * cost * 24);
//    BOOST_TEST(result->avgdata.weekly[0] < rhs * cost * 24 * 7);
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsWeeklyLess)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto [_ ,link] = prepare(study, rhs, BindingConstraint::typeWeekly, BindingConstraint::opLess);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.weekly[0] < rhs * cost * 7);
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsDailyGreater)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto [_ ,link] = prepare(study, rhs, BindingConstraint::typeDaily, BindingConstraint::opEquality);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.daily[0] > rhs * cost);
//    BOOST_TEST(result->avgdata.weekly[0] > rhs * cost * 7);
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(two_year_one_ts__Binding_ConstraintsWeekly)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs = 0.3;
//    auto cost = 1;
//    auto nbYear = 2;
//    auto [BC ,link] = prepare(study, rhs, BindingConstraint::typeWeekly, BindingConstraint::opEquality, nbYear);
//
//    study->bindingConstraints.resizeAllTimeseriesNumbers(2);
//    auto& ts_numbers = study->bindingConstraints.timeSeriesNumbers[BC->group()];
//    ts_numbers.timeseriesNumbers.fill(10);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(two_mc_year_two_ts__Binding_Constraints_Hourly)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs_ts1 = 0.3;
//    auto rhs_ts2 = 0.6;
//    auto cost = 1;
//    auto nbYears = 2;
//    auto [BC, link] = prepare(study, rhs_ts1, BindingConstraint::typeHourly, BindingConstraint::opEquality, nbYears);
//
//    //Define years weight
//    std::vector<float> yearsWeight;
//    yearsWeight.assign(nbYears, 1);
//    yearsWeight[0] = 4.f;	yearsWeight[1] = 10.f;
//
//    float yearSum = defineYearsWeight(study,yearsWeight);
//
//    //Add one TS
//    auto& ts_numbers = study->bindingConstraints.timeSeriesNumbers[BC->group()];
//    BC->RHSTimeSeries().resize(2, 8760);
//    BC->RHSTimeSeries().fillColumn(0, rhs_ts1);
//    BC->RHSTimeSeries().fillColumn(1, rhs_ts2);
//    study->bindingConstraints.resizeAllTimeseriesNumbers(2);
//    ts_numbers.timeseriesNumbers.fill(0);
//    //Create scenario rules
//
//    ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(study);
//    pRules->binding_constraints.setData(BC->group(), 0, 1);
//    pRules->binding_constraints.setData(BC->group(), 1, 2);
//
//    double averageLoad = (rhs_ts1 * 4.f + rhs_ts2 * 10.f) / yearSum;
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.hourly[0] == averageLoad, tt::tolerance(0.001));
//    BOOST_TEST(result->avgdata.daily[0] == averageLoad * 24, tt::tolerance(0.001));
//    BOOST_TEST(result->avgdata.weekly[0] == averageLoad * 24 * 7, tt::tolerance(0.001));
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}
//
//BOOST_AUTO_TEST_CASE(two_mc_year_one_ts__Binding_Constraints_Hourly)
//{
//    //Create study
//    Study::Ptr study = std::make_shared<Study>(true); // for the solver
//    auto rhs_ts1 = 0.3;
//    auto cost = 1;
//    auto nbYears = 2;
//    auto [BC, link] = prepare(study, rhs_ts1, BindingConstraint::typeHourly, BindingConstraint::opEquality, nbYears);
//
//    //Define years weight
//    std::vector<float> yearsWeight;
//    yearsWeight.assign(nbYears, 1);
//    yearsWeight[0] = 4.f;	yearsWeight[1] = 10.f;
//
//    float yearSum = defineYearsWeight(study,yearsWeight);
//
//    //Add one TS
//    auto& ts_numbers = study->bindingConstraints.timeSeriesNumbers[BC->group()];
//    BC->RHSTimeSeries().resize(1, 8760);
//    BC->RHSTimeSeries().fillColumn(0, rhs_ts1);
//    study->bindingConstraints.resizeAllTimeseriesNumbers(nbYears);
//    ts_numbers.timeseriesNumbers.fill(0);
//    //Create scenario rules
//
//    ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(study);
//    pRules->binding_constraints.setData(BC->group(), 0, 1);
//    pRules->binding_constraints.setData(BC->group(), 1, 10);
//
//    //Launch simulation
//    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(study);
//
//    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
//    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
//    BOOST_TEST(result->avgdata.hourly[0] == rhs_ts1, tt::tolerance(0.001));
//    BOOST_TEST(result->avgdata.daily[0] == rhs_ts1 * 24, tt::tolerance(0.001));
//    BOOST_TEST(result->avgdata.weekly[0] == rhs_ts1 * 24 * 7, tt::tolerance(0.001));
//
//    //Clean simulation
//    cleanSimulation(study, simulation);
//}

BOOST_AUTO_TEST_SUITE_END()
