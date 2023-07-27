#define BOOST_TEST_MODULE test-end-to-end tests_binding_constraints
#define WIN32_LEAN_AND_MEAN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "utils.h"
#include "simulation.h"

#include "antares/study/study.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;


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

    Area* area1 = addAreaToStudy("Area 1");
    Area* area2 = addAreaToStudy("Area 2");

    auto loadTSconfig = std::make_shared<TimeSeriesConfig<Matrix<double, int32_t>>>(area1->load.series->timeSeries);
    loadTSconfig->setNumberColumns(1);
    loadTSconfig->fillColumnWith(0, 0);

    loadTSconfig->setTargetTSmatrix(area2->load.series->timeSeries);
    loadTSconfig->setNumberColumns(1);
    loadTSconfig->fillColumnWith(0, 100);

    link = AreaAddLinkBetweenAreas(area1, area2);

    configureLinkCapacities(link);

    cluster = addClusterToArea(area1, "some cluster");

    ThermalClusterConfig clusterConfig(cluster);
    clusterConfig.setNominalCapacity(100.);
    clusterConfig.setAvailablePower(0, 100.);
    clusterConfig.setCosts(50.);
    clusterConfig.setUnitCount(1);
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


// =======================================================
// Study fixture containing a BC on the thermal cluster 
// =======================================================

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
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 90.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    BOOST_TEST(output->flow(link).hour(0) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(weekly_BC_restricts_link_direct_capacity_to_50)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeWeekly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 50.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

  
    simulation->create();
    simulation->run();

    unsigned int nbDaysInWeek = 7;
    BOOST_TEST(output->flow(link).week(0) == rhsValue * nbDaysInWeek, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(daily_BC_restricts_link_direct_capacity_to_60)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 60.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    BOOST_TEST(output->flow(link).day(0) == rhsValue, tt::tolerance(0.001));
}


BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_less_than_90)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 90.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    BOOST_TEST(output->flow(link).hour(100) <= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(Daily_BC_restricts_link_direct_capacity_to_greater_than_80)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opGreater);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 80.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    BOOST_TEST(output->flow(link).hour(100) >= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_ON_A_CLUSTER, StudyWithBConCluster)

BOOST_AUTO_TEST_CASE(Hourly_BC_restricts_cluster_generation_to_90)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(1);
    double rhsValue = 90.;
    bcRHStsConfig.fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    BOOST_TEST(output->thermalGeneration(cluster.get()).hour(10) == rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(TESTING_BC_RHS_SCENARIZATION_WHEN_BC_ON_A_LINK, StudyWithBConLink)


BOOST_AUTO_TEST_CASE(On_year_2__RHS_TS_number_2_is_taken_into_account)
{
    setNumberMCyears(2);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(2);
    double bcGroupRHS1 = 90.;
    double bcGroupRHS2 = 70.;
    bcRHStsConfig.fillColumnWith(0, bcGroupRHS1);
    bcRHStsConfig.fillColumnWith(1, bcGroupRHS2);

    ScenarioBuilderRule scenarioBuilderRule(study);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 0, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 1, 2);

    simulation->create();
    playOnlyYear(1);
    simulation->run();

    BOOST_TEST(output->flow(link).hour(0) == bcGroupRHS2, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_CASE(On_year_9__RHS_TS_number_4_is_taken_into_account)
{
    setNumberMCyears(10);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfig<Matrix<>> bcRHStsConfig(BC->RHSTimeSeries());
    bcRHStsConfig.setNumberColumns(7);
    bcRHStsConfig.fillColumnWith(0, 10.);
    bcRHStsConfig.fillColumnWith(1, 20.);
    bcRHStsConfig.fillColumnWith(2, 30.);
    bcRHStsConfig.fillColumnWith(3, 40.);
    bcRHStsConfig.fillColumnWith(4, 50.);
    bcRHStsConfig.fillColumnWith(5, 60.);
    bcRHStsConfig.fillColumnWith(6, 70.);

    ScenarioBuilderRule scenarioBuilderRule(study);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 0, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 1, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 2, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 3, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 4, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 5, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 6, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 7, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 8, 4);  // Here year 9
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 9, 1);

    simulation->create();
    playOnlyYear(8);
    simulation->run();

    BOOST_TEST(output->flow(link).hour(0) == 40., tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
