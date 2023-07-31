#define BOOST_TEST_MODULE test-end-to-end tests_binding_constraints
#define WIN32_LEAN_AND_MEAN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <utility>
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

void setNumberMCyears(Study::Ptr study, unsigned int nbYears)
{
    study->parameters.resetPlaylist(nbYears);
    study->bindingConstraintsGroups.resizeAllTimeseriesNumbers(nbYears);
}

void simulationBetweenDays(Study::Ptr study, const unsigned int firstDay, const unsigned int lastDay)
{
    study->parameters.simulationDays.first = firstDay;
    study->parameters.simulationDays.end = lastDay;
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
    const double linkCapacityInfinite = +std::numeric_limits<double>::infinity();
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


// -------------------------------
// Simulation results retrieval
// -------------------------------

Variable::Storage<Variable::Economy::VCardFlowLinear>::ResultsType*
retrieveLinkResults(const std::shared_ptr<ISimulation<Economy>>& simulation, AreaLink* link)
{
    typename Variable::Storage<Variable::Economy::VCardFlowLinear>::ResultsType* result = nullptr;
    simulation->variables.retrieveResultsForLink<Variable::Economy::VCardFlowLinear>(&result, link);
    return result;
}

double getLinkFlowAthour(const std::shared_ptr<ISimulation<Economy>>& simulation, AreaLink* link, unsigned int hour)
{
    // There is a problem here :
    //    we cannot easly retrieve the hourly flow for a link and a year :
    //    - Functions retrieveHourlyResultsForCurrentYear are not coded everywhere it should.
    //    - Even if those functions were correctly implemented, there is another problem :
    //      Each year results erase results of previous year, how can we retrieve results of year 1
    //      if 2 year were run ?
    //    We should be able to run each year independently, which is not possible now.
    //    A workaround is to retrieve syntheses, and that's what we do here.

    auto result = retrieveLinkResults(simulation, link);
    return result->avgdata.hourly[hour];
}

double getLinkFlowForWeek(const std::shared_ptr<ISimulation<Economy>>& simulation, AreaLink* link, unsigned int week)
{
    auto result = retrieveLinkResults(simulation, link);
    return result->avgdata.weekly[week];
}

double getLinkFlowForDay(const std::shared_ptr<ISimulation<Economy>>& simulation, AreaLink* link, unsigned int day)
{
    auto result = retrieveLinkResults(simulation, link);
    return result->avgdata.daily[day];
}


// =================
// Helper classes
// =================

// -----------------------
// BC rhs configuration
// -----------------------
class BCrhsConfig
{
public:
    BCrhsConfig() = delete;
    BCrhsConfig(std::shared_ptr<BindingConstraint> BC, unsigned int nbOfTimeSeries);
    void fillTimeSeriesWith(unsigned int TSnumber, double rhsValue);

private:
    std::shared_ptr<BindingConstraint> BC_;
    unsigned int nbOfTimeSeries_ = 0;
};

BCrhsConfig::BCrhsConfig(std::shared_ptr<BindingConstraint> BC, unsigned int nbOfTimeSeries)
    : BC_(std::move(BC)), nbOfTimeSeries_(nbOfTimeSeries)
{
    BC_->RHSTimeSeries().resize(nbOfTimeSeries_, 8760);
}

void BCrhsConfig::fillTimeSeriesWith(unsigned int TSnumber, double rhsValue)
{
    BOOST_CHECK(TSnumber < nbOfTimeSeries_);
    BC_->RHSTimeSeries().fillColumn(TSnumber, rhsValue);
}


// --------------------------------------
// BC group TS number configuration
// --------------------------------------
class BCgroupScenarioBuilder
{
public:
    BCgroupScenarioBuilder() = delete;
    BCgroupScenarioBuilder(Study::Ptr study, unsigned int nbYears);
    void yearGetsTSnumber(const std::string& groupName, unsigned int year, unsigned int TSnumber);

private:
    unsigned int nbYears_ = 0;
    ScenarioBuilder::Rules::Ptr rules_;
};

BCgroupScenarioBuilder::BCgroupScenarioBuilder(Study::Ptr study, unsigned int nbYears)
    : nbYears_(nbYears)

{
    rules_ = createScenarioRules(std::move(study));
}

void BCgroupScenarioBuilder::yearGetsTSnumber(const std::string& groupName, unsigned int year, unsigned int TSnumber)
{
    BOOST_CHECK(year < nbYears_);
    rules_->binding_constraints.setData(groupName, year, TSnumber + 1);
}

// =====================
// Simulation handler
// =====================
class SimulationHandler
{
public:
    explicit SimulationHandler(std::shared_ptr<Study> study)
        : study_(std::move(study))
    {}
    ~SimulationHandler() = default;
    void create();
    void run() { simulation_->run(); }
    std::shared_ptr<ISimulation<Economy>> get() { return simulation_; }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    NullDurationCollector nullDurationCollector_;
    Settings settings_;
    std::shared_ptr<Study> study_;
};

void SimulationHandler::create()
{
    BOOST_CHECK(study_);
    BOOST_CHECK(study_->initializeRuntimeInfos());
    addScratchpadToEachArea(study_);

    simulation_ = std::make_shared<ISimulation<Economy>>(*study_,
                                                         settings_,
                                                         &nullDurationCollector_);

    // Allocate arrays for time series
    SIM_AllocationTableaux(*study_);
}

SimulationHandler::~SimulationHandler()
= default;

// ===============
// The fixture
// ===============
struct Fixture {
    Fixture();
    void giveWeigthOnlyToYear(unsigned int year);

    // Data members
    AreaLink* link = nullptr;
    std::shared_ptr<BindingConstraint> BC;
    std::shared_ptr<SimulationHandler> simulation;
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
    simulation = std::make_shared<SimulationHandler>(study);

    initializeStudy(study);
    simulationBetweenDays(study, 0, 7);

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
    study->bindingConstraintsGroups.buildFrom(study->bindingConstraints);
};

void Fixture::giveWeigthOnlyToYear(unsigned int year)
{
    // Set all years weight to zero
    unsigned int nbYears = study->parameters.nbYears;
    for (unsigned int y = 0; y < nbYears; y++)
        study->parameters.setYearWeight(y, 0.);

    // Set one year's weight to 1
    study->parameters.setYearWeight(year, 1.);

    // Activate playlist, otherwise previous sets won't have any effect
    study->parameters.userPlaylist = true;
}


BOOST_FIXTURE_TEST_SUITE(TESTS_ON_BINDING_CONSTRAINTS, Fixture)

BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_90)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 90.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    giveWeigthOnlyToYear(0);
    simulation->run();

    unsigned int hour = 0;
    BOOST_TEST(getLinkFlowAthour(simulation->get(), link, hour) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(weekly_BC_restricts_link_direct_capacity_to_50)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeWeekly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 50.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    giveWeigthOnlyToYear(0);
    simulation->run();

    unsigned int week = 0;
    unsigned int nbDaysInWeek = 7;
    BOOST_TEST(getLinkFlowForWeek(simulation->get(), link, week) == rhsValue * nbDaysInWeek, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(daily_BC_restricts_link_direct_capacity_to_60)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 60.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    giveWeigthOnlyToYear(0);
    simulation->run();

    unsigned int day = 0;
    BOOST_TEST(getLinkFlowForDay(simulation->get(), link, day) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_less_than_90)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 90.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    giveWeigthOnlyToYear(0);
    simulation->run();

    unsigned int hour = 100;
    BOOST_TEST(getLinkFlowAthour(simulation->get(), link, hour) <= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(Daily_BC_restricts_link_direct_capacity_to_greater_than_80)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opGreater);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 80.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    giveWeigthOnlyToYear(0);
    simulation->run();

    unsigned int hour = 100;
    BOOST_TEST(getLinkFlowAthour(simulation->get(), link, hour) >= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(TESTS_ON_BC_RHS_SCENARIZATION, Fixture)


BOOST_AUTO_TEST_CASE(On_year_2__RHS_TS_number_2_is_taken_into_account)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 2;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 2;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);
    double bcGroupRHS1 = 90.;
    double bcGroupRHS2 = 70.;
    bcRHSconfig.fillTimeSeriesWith(0, bcGroupRHS1);
    bcRHSconfig.fillTimeSeriesWith(1, bcGroupRHS2);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 1, 1);

    simulation->create();
    giveWeigthOnlyToYear(1);
    simulation->run();

    unsigned int hour = 0;
    BOOST_TEST(getLinkFlowAthour(simulation->get(), link, hour) == bcGroupRHS2, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(On_year_9__RHS_TS_number_4_is_taken_into_account)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 10;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->mutateTypeWithoutCheck(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 7;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);
    bcRHSconfig.fillTimeSeriesWith(0, 10.);
    bcRHSconfig.fillTimeSeriesWith(1, 20.);
    bcRHSconfig.fillTimeSeriesWith(2, 30.);
    bcRHSconfig.fillTimeSeriesWith(3, 40.);
    bcRHSconfig.fillTimeSeriesWith(4, 50.);
    bcRHSconfig.fillTimeSeriesWith(5, 60.);
    bcRHSconfig.fillTimeSeriesWith(6, 70.);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 1, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 2, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 3, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 4, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 5, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 6, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 7, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 8, 3); // Here year 9
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 9, 0);

    simulation->create();
    giveWeigthOnlyToYear(8);
    simulation->run();

    unsigned int hour = 0;
    BOOST_TEST(getLinkFlowAthour(simulation->get(), link, hour) == 40., tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
