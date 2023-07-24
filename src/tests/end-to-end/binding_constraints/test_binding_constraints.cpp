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

void addLoadToArea(Area* area, double loadInArea)
{
    unsigned int loadNumberTS = 1;
    area->load.series->timeSeries.resize(loadNumberTS, HOURS_PER_YEAR);
    area->load.series->timeSeries.fill(loadInArea);
}

void configureCluster(std::shared_ptr<ThermalCluster> cluster)
{
    double availablePower = 100.0;
    double maximumPower = 100.0;
    double clusterCost = 50.;
    unsigned int unitCount = 1;

    cluster->unitCount = unitCount;
    cluster->nominalCapacity = maximumPower;

    cluster->marginalCost = clusterCost;

    // Must define market bid cost otherwise all production is used
    cluster->marketBidCost = clusterCost;

    cluster->minStablePower = 0.0;
    cluster->series->timeSeries.fill(availablePower);
}

// =====================
// Simulation handler
// =====================
class SimulationHandler
{
public:
    SimulationHandler(std::shared_ptr<Study> study) 
        : study_(study)
    {}
    ~SimulationHandler() = default;
    void create();
    void run() { simulation_->run(); }
    std::shared_ptr<ISimulation<Economy>>& get() { return simulation_; }

private:
    std::shared_ptr<ISimulation<Economy>> simulation_;
    NullDurationCollector nullDurationCollector_;
    Settings settings_;
    std::shared_ptr<Study> study_;
};

void SimulationHandler::create()
{
    BOOST_CHECK(study_->initializeRuntimeInfos());
    addScratchpadToEachArea(study_);

    simulation_ = std::make_shared<ISimulation<Economy>>(*study_,
                                                         settings_,
                                                         &nullDurationCollector_);
    SIM_AllocationTableaux();
}

// =========================
// Basic study builder
// =========================

struct StudyBuilder
{
    StudyBuilder();

    void simulationBetweenDays(const unsigned int firstDay, const unsigned int lastDay);
    Area* addAreaToStudy(const std::string& areaName);
    void giveWeigthOnlyToYear(unsigned int year);

    // Data members
    std::shared_ptr<Study> study;
    std::shared_ptr<SimulationHandler> simulation;
    std::shared_ptr<OutputRetriever> output;
};

StudyBuilder::StudyBuilder()
{
    // Make logs shrink to errors (and higher) only
    logs.verbosityLevel = Logs::Verbosity::Error::level;

    study = std::make_shared<Study>();
    simulation = std::make_shared<SimulationHandler>(study);

    initializeStudy(study);
    output = std::make_shared<OutputRetriever>(simulation->get());
}

void StudyBuilder::simulationBetweenDays(const unsigned int firstDay, const unsigned int lastDay)
{
    study->parameters.simulationDays.first = firstDay;
    study->parameters.simulationDays.end = lastDay;
}

void StudyBuilder::giveWeigthOnlyToYear(unsigned int year)
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

Area* StudyBuilder::addAreaToStudy(const std::string& areaName)
{
    Area* area = addAreaToListOfAreas(study->areas, areaName);
    BOOST_CHECK(area != NULL);

    // Default values for the area
    area->createMissingData();
    area->resetToDefaultValues();

    // Temporary : we want to give a high unsupplied or spilled energy costs.
    // Which cost should we give ?
    area->thermal.unsuppliedEnergyCost = 1000.0;
    area->thermal.spilledEnergyCost = 1000.0;

    study->areas.rebuildIndexes();

    return area;
}


// =================================
// The Basic fixture fot BC tests
// =================================
struct StudyForBCTest : public StudyBuilder
{
    using StudyBuilder::StudyBuilder;
    StudyForBCTest();

    // Data members
    AreaLink* link = nullptr;
    std::shared_ptr<ThermalCluster> cluster;
    std::shared_ptr<BindingConstraint> BC;
};

// =======================================
// The basic fixture's member functions
// =======================================

StudyForBCTest::StudyForBCTest()
{
    simulationBetweenDays(0, 7);

    Area* area1 = addAreaToStudy("Area 1");
    Area* area2 = addAreaToStudy("Area 2");

    double loadInAreaOne = 0.;
    addLoadToArea(area1, loadInAreaOne);

    double loadInAreaTwo = 100.;
    addLoadToArea(area2, loadInAreaTwo);

    link = AreaAddLinkBetweenAreas(area1, area2);

    configureLinkCapacities(link);

    cluster = addClusterToArea(area1, "some cluster");
    configureCluster(cluster);
};

// ==============================================
// Study fixture containing a BC on the link 
// ==============================================

struct StudyWithBConLink : public StudyForBCTest
{
    using StudyForBCTest::StudyForBCTest;
    StudyWithBConLink();
};

StudyWithBConLink::StudyWithBConLink()
{
    BC = addBindingConstraints(study, "BC1", "Group1");
    BC->weight(link, 1);
    BC->enabled(true);
}


// ==============================================
// Study fixture containing a BC on the link 
// ==============================================

struct StudyWithBConCluster : public StudyForBCTest
{
    using StudyForBCTest::StudyForBCTest;
    StudyWithBConCluster();
};

StudyWithBConCluster::StudyWithBConCluster()
{
    BC = addBindingConstraints(study, "BC1", "Group1");
    BC->weight(cluster.get(), 1);
    BC->enabled(true);
}


BOOST_FIXTURE_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_ON_A_LINK, StudyWithBConLink)

BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_90)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 90.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);
        
    simulation->create();
    simulation->run();

    unsigned int hour = 0;
    BOOST_TEST(output->flow(link).hour(hour) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(weekly_BC_restricts_link_direct_capacity_to_50)
{
    // Study parameters varying depending on the test 
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeWeekly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 50.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);
  
    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    simulation->run();

    unsigned int week = 0;
    unsigned int nbDaysInWeek = 7;
    BOOST_TEST(output->flow(link).week(week) == rhsValue * nbDaysInWeek, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(daily_BC_restricts_link_direct_capacity_to_60)
{
    // Study parameters varying depending on the test 
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 60.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    simulation->run();

    unsigned int day = 0;
    BOOST_TEST(output->flow(link).day(day) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_less_than_90)
{
    // Study parameters varying depending on the test 
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 90.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    simulation->run();

    unsigned int hour = 100;
    BOOST_TEST(output->flow(link).hour(hour) <= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(Daily_BC_restricts_link_direct_capacity_to_greater_than_80)
{
    // Study parameters varying depending on the test 
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opGreater);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 80.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    simulation->run();

    unsigned int hour = 100;
    BOOST_TEST(output->flow(link).hour(hour) >= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_ON_A_CLUSTER, StudyWithBConCluster)

BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_cluster_generation_to_90)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 1;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    unsigned int numberOfTS = 1;
    BCrhsConfig bcRHSconfig(BC, numberOfTS);

    double rhsValue = 90.;
    bcRHSconfig.fillTimeSeriesWith(0, rhsValue);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);

    simulation->create();
    simulation->run();

    unsigned int hour = 10;
    BOOST_TEST(output->thermalGeneration(cluster.get()).hour(hour) == rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(TESTING_BC_RHS_SCENARIZATION_WHEN_BC_ON_A_LINK, StudyWithBConLink)


BOOST_AUTO_TEST_CASE(On_year_2__RHS_TS_number_2_is_taken_into_account)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 2;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeHourly);
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
    BOOST_TEST(output->flow(link).hour(hour) == bcGroupRHS2, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(On_year_9__RHS_TS_number_4_is_taken_into_account)
{
    // Study parameters varying depending on the test
    unsigned int nbYears = 10;
    setNumberMCyears(study, nbYears);

    // Binding constraint parameter varying depending on the test
    BC->setTimeGranularity(BindingConstraint::typeHourly);
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
    BOOST_TEST(output->flow(link).hour(hour) == 40., tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
