#define BOOST_TEST_MODULE test-end-to-end tests_binding_constraints
#define WIN32_LEAN_AND_MEAN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "utils.h"
#include "simulation.h"

#include "antares/study/study.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;


// ===============================
// Helper functions for BC tests
// ===============================

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


// =================================
// The Basic fixture for BC tests
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

StudyForBCTest::StudyForBCTest()
{
    simulationBetweenDays(0, 7);

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

    double loadInAreaOne = 0.;
    addLoadToArea(area1, loadInAreaOne);

    double loadInAreaTwo = 100.;
    addLoadToArea(area2, loadInAreaTwo);

    link = AreaAddLinkBetweenAreas(area1, area2);

    configureLinkCapacities(link);

    cluster = addClusterToArea(area1, "some cluster");
    configureCluster(cluster);
};

// ======================================================
// Study fixture containing a BC on the link capacity
// ======================================================

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
    std::shared_ptr<ISimulation<Economy>> get() { return simulation_; }

struct StudyWithBConCluster : public StudyForBCTest
{
    BOOST_CHECK(study_->initializeRuntimeInfos());
    addScratchpadToEachArea(study_);

    simulation_ = std::make_shared<ISimulation<Economy>>(*study_,
                                                         settings_,
                                                         &nullDurationCollector_);

    // Allocate arrays for time series
    SIM_AllocationTableaux();
}

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, rhsValue);

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
    bcRHSconfig.fillRHStimeSeriesWith(0, bcGroupRHS1);
    bcRHSconfig.fillRHStimeSeriesWith(1, bcGroupRHS2);

    BCgroupScenarioBuilder bcGroupScenarioBuilder(study, nbYears);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 0, 0);
    bcGroupScenarioBuilder.yearGetsTSnumber(BC->group(), 1, 1);

    simulation->create();
    playOnlyYear(1);
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
    bcRHSconfig.fillRHStimeSeriesWith(0, 10.);
    bcRHSconfig.fillRHStimeSeriesWith(1, 20.);
    bcRHSconfig.fillRHStimeSeriesWith(2, 30.);
    bcRHSconfig.fillRHStimeSeriesWith(3, 40.);
    bcRHSconfig.fillRHStimeSeriesWith(4, 50.);
    bcRHSconfig.fillRHStimeSeriesWith(5, 60.);
    bcRHSconfig.fillRHStimeSeriesWith(6, 70.);

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
    playOnlyYear(8);
    simulation->run();

    unsigned int hour = 0;
    BOOST_TEST(output->flow(link).hour(hour) == 40., tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
