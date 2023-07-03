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

Area* addArea(Study::Ptr pStudy, const std::string& areaName, int nbTS)
{
    Area* pArea = pStudy->areaAdd(areaName);

    BOOST_CHECK(pArea != NULL);

    //Need to add unsupplied energy cost constraint so load is respected
    pArea->thermal.unsuppliedEnergyCost = 10000.0;
    pArea->spreadUnsuppliedEnergyCost	= 0.01;

    //Define default load
    pArea->load.series->timeSeries.resize(nbTS, HOURS_PER_YEAR);
    pArea->load.series->timeSeries.fill(0.0);

    return pArea;
}

std::shared_ptr<ThermalCluster> addCluster(Area* pArea, const std::string& clusterName, double maximumPower, double cost, int nbTS, int unitCount)
{
    auto pCluster = std::make_shared<ThermalCluster>(pArea);
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
    pCluster->series->timeSeries.resize(nbTS, HOURS_PER_YEAR);
    pCluster->series->timeSeries.fill(0.0);

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

Solver::Simulation::ISimulation< Solver::Simulation::Economy >* runSimulation(Study::Ptr pStudy)
{
    // Runtime data dedicated for the solver
    BOOST_CHECK(pStudy->initializeRuntimeInfos());

    for(auto [_, area]: pStudy->areas) {
        for (unsigned int i = 0; i<pStudy->maxNbYearsInParallel ;++i) {
            area->scratchpad.push_back(AreaScratchpad(*pStudy->runtime, *area));
        }
    }

    Settings pSettings;
    pSettings.tsGeneratorsOnly = false;
    pSettings.noOutput = false;

    //Launch simulation
    Benchmarking::NullDurationCollector nullDurationCollector;
    Solver::Simulation::ISimulation<Solver::Simulation::Economy> *simulation = new Solver::Simulation::ISimulation<Solver::Simulation::Economy>(
      *pStudy, pSettings, &nullDurationCollector);

    // Allocate all arrays
    SIM_AllocationTableaux();

    // Let's go
    simulation->run();

    return simulation;
}

void prepareStudy(int nbYears, int nbTS, Study::Ptr &pStudy, Area *&area1,
                  AreaLink *&link) {
    Area *area2 = addArea(pStudy, "Area 2", nbTS);
    area1= addArea(pStudy, "Area 1", nbTS);
    link= AreaAddLinkBetweenAreas(area1, area2);//Prepare study
    prepareStudy(pStudy, nbYears);
    auto* area3 = addArea(pStudy, "Area 3", nbTS);
    link->directCapacities.resize(1, 8760);
    link->indirectCapacities.resize(1, 8760);
    link->directCapacities.fill(1);
    link->indirectCapacities.fill(1);
    auto link2 = AreaAddLinkBetweenAreas(area2, area3);
    auto link3 = AreaAddLinkBetweenAreas(area1, area3);
    link2->directCapacities.resize(1, 8760);
    link2->directCapacities.resize(1, 8760);
    link2->directCapacities.fill(1);
    link2->indirectCapacities.fill(1);
    link3->directCapacities.resize(1, 8760);
    link3->directCapacities.resize(1, 8760);
    link3->directCapacities.fill(1);
    link3->indirectCapacities.fill(1);

    //Add thermal  cluster
    double availablePower = 50000.0;
    double maximumPower = 100000.0;
    auto pCluster = addCluster(area1, "Cluster 1", maximumPower, 1, nbTS);

    //Initialize time series
    pCluster->series->timeSeries.fillColumn(0, availablePower);
}

BOOST_AUTO_TEST_SUITE(tests_end2end_binding_constraints)

auto prepare(Study::Ptr pStudy, double rhs, BindingConstraint::Type type, BindingConstraint::Operator op, int nbYears = 1) {
    pStudy->resultWriter = std::make_shared<NoOPResultWriter>();
    //On year  and one TS
    int nbTS = 1;

    Area* area1;
    AreaLink* link;

    prepareStudy(nbYears, nbTS, pStudy, area1, link);

    //Add BC
    auto BC = addBindingConstraints(pStudy, "BC1", "Group1");
    BC->weight(link, 1);
    BC->enabled(true);
    BC->mutateTypeWithoutCheck(type);
    BC->operatorType(op);
    auto& ts_numbers = pStudy->bindingConstraints.groupToTimeSeriesNumbers[BC->group()];
    BC->RHSTimeSeries().resize(1, 8760);
    BC->RHSTimeSeries().fill(rhs);
    pStudy->bindingConstraints.resizeAllTimeseriesNumbers(1);
    ts_numbers.timeseriesNumbers.fill(0);
    return std::pair(BC, link);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_Constraints_Hourly)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeHourly, BindingConstraint::opEquality);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.hourly[0] == rhs * cost, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.daily[0] == rhs * cost * 24, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 24 * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsWeekly)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeWeekly, BindingConstraint::opEquality);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsDaily)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeDaily, BindingConstraint::opEquality);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.daily[0] == rhs * cost, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_Constraints_HourlyLess)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeHourly, BindingConstraint::opLess);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.hourly[0] < rhs * cost);
    BOOST_TEST(result->avgdata.daily[0] < rhs * cost * 24);
    BOOST_TEST(result->avgdata.weekly[0] < rhs * cost * 24 * 7);

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsWeeklyLess)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeWeekly, BindingConstraint::opLess);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.weekly[0] < rhs * cost * 7);

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(one_mc_year_one_ts__Binding_ConstraintsDailyGreater)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto [_ ,link] = prepare(pStudy, rhs, BindingConstraint::typeDaily, BindingConstraint::opEquality);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.daily[0] > rhs * cost);
    BOOST_TEST(result->avgdata.weekly[0] > rhs * cost * 7);

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(two_year_one_ts__Binding_ConstraintsWeekly)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs = 0.3;
    auto cost = 1;
    auto nbYear = 2;
    auto [BC ,link] = prepare(pStudy, rhs, BindingConstraint::typeWeekly, BindingConstraint::opEquality, nbYear);

    pStudy->bindingConstraints.resizeAllTimeseriesNumbers(2);
    auto& ts_numbers = pStudy->bindingConstraints.groupToTimeSeriesNumbers[BC->group()];
    ts_numbers.timeseriesNumbers.fill(10);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.weekly[0] == rhs * cost * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(two_mc_year_two_ts__Binding_Constraints_Hourly)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs_ts1 = 0.3;
    auto rhs_ts2 = 0.6;
    auto cost = 1;
    auto nbYears = 2;
    auto [BC, link] = prepare(pStudy, rhs_ts1, BindingConstraint::typeHourly, BindingConstraint::opEquality, nbYears);

    //Define years weight
    std::vector<float> yearsWeight;
    yearsWeight.assign(nbYears, 1);
    yearsWeight[0] = 4.f;	yearsWeight[1] = 10.f;

    float yearSum = defineYearsWeight(pStudy,yearsWeight);

    //Add one TS
    auto& ts_numbers = pStudy->bindingConstraints.groupToTimeSeriesNumbers[BC->group()];
    BC->RHSTimeSeries().resize(2, 8760);
    BC->RHSTimeSeries().fillColumn(0, rhs_ts1);
    BC->RHSTimeSeries().fillColumn(1, rhs_ts2);
    pStudy->bindingConstraints.resizeAllTimeseriesNumbers(2);
    ts_numbers.timeseriesNumbers.fill(0);
    //Create scenario rules

    ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(pStudy);
    pRules->binding_constraints.setData(BC->group(), 0, 1);
    pRules->binding_constraints.setData(BC->group(), 1, 2);

    double averageLoad = (rhs_ts1 * 4.f + rhs_ts2 * 10.f) / yearSum;

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.hourly[0] == averageLoad, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.daily[0] == averageLoad * 24, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.weekly[0] == averageLoad * 24 * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_CASE(two_mc_year_one_ts__Binding_Constraints_Hourly)
{
    //Create study
    Study::Ptr pStudy = std::make_shared<Study>(true); // for the solver
    auto rhs_ts1 = 0.3;
    auto cost = 1;
    auto nbYears = 2;
    auto [BC, link] = prepare(pStudy, rhs_ts1, BindingConstraint::typeHourly, BindingConstraint::opEquality, nbYears);

    //Define years weight
    std::vector<float> yearsWeight;
    yearsWeight.assign(nbYears, 1);
    yearsWeight[0] = 4.f;	yearsWeight[1] = 10.f;

    float yearSum = defineYearsWeight(pStudy,yearsWeight);

    //Add one TS
    auto& ts_numbers = pStudy->bindingConstraints.groupToTimeSeriesNumbers[BC->group()];
    BC->RHSTimeSeries().resize(1, 8760);
    BC->RHSTimeSeries().fillColumn(0, rhs_ts1);
    pStudy->bindingConstraints.resizeAllTimeseriesNumbers(nbYears);
    ts_numbers.timeseriesNumbers.fill(0);
    //Create scenario rules

    ScenarioBuilder::Rules::Ptr pRules = createScenarioRules(pStudy);
    pRules->binding_constraints.setData(BC->group(), 0, 1);
    pRules->binding_constraints.setData(BC->group(), 1, 10);

    //Launch simulation
    Solver::Simulation::ISimulation< Solver::Simulation::Economy >* simulation = runSimulation(pStudy);

    typename Antares::Solver::Variable::Storage<Solver::Variable::Economy::VCardFlowLinear>::ResultsType *result = nullptr;
    simulation->variables.retrieveResultsForLink<Solver::Variable::Economy::VCardFlowLinear>(&result, link);
    BOOST_TEST(result->avgdata.hourly[0] == rhs_ts1, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.daily[0] == rhs_ts1 * 24, tt::tolerance(0.001));
    BOOST_TEST(result->avgdata.weekly[0] == rhs_ts1 * 24 * 7, tt::tolerance(0.001));

    //Clean simulation
    cleanSimulation(pStudy, simulation);
}

BOOST_AUTO_TEST_SUITE_END()
