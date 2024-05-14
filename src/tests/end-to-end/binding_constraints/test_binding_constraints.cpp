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
#define BOOST_TEST_MODULE test - end - to - end tests_binding_constraints
#define WIN32_LEAN_AND_MEAN
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include "utils.h"

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

// =================================
// The Basic fixture for BC tests
// =================================
struct StudyForBCTest: public StudyBuilder
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

    TimeSeriesConfigurer(area1->load.series.timeSeries).setColumnCount(1).fillColumnWith(0, 0);

    TimeSeriesConfigurer(area2->load.series.timeSeries).setColumnCount(1).fillColumnWith(0, 100);

    link = AreaAddLinkBetweenAreas(area1, area2);

    configureLinkCapacities(link);

    cluster = addClusterToArea(area1, "some cluster");

    ThermalClusterConfig(cluster.get())
      .setNominalCapacity(100.)
      .setAvailablePower(0, 100.)
      .setCosts(50.)
      .setUnitCount(1);
}

// ======================================================
// Study fixture containing a BC on the link capacity
// ======================================================

struct StudyWithBConLink: public StudyForBCTest
{
    using StudyForBCTest::StudyForBCTest;
    StudyWithBConLink();
};

StudyWithBConLink::StudyWithBConLink()
{
    BC = addBindingConstraints(*study, "BC1", "Group1");
    BC->weight(link, 1);
    BC->enabled(true);
}

// =======================================================
// Study fixture containing a BC on the thermal cluster
// =======================================================

struct StudyWithBConCluster: public StudyForBCTest
{
    using StudyForBCTest::StudyForBCTest;
    StudyWithBConCluster();
};

StudyWithBConCluster::StudyWithBConCluster()
{
    BC = addBindingConstraints(*study, "BC1", "Group1");
    BC->weight(cluster.get(), 1);
    BC->enabled(true);
}

BOOST_AUTO_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_ON_A_LINK)

BOOST_FIXTURE_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_90, StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    double rhsValue = 90.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());

    BOOST_TEST(output.flow(link).hour(0) == rhsValue, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(weekly_BC_restricts_link_direct_capacity_to_50, StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeWeekly);
    BC->operatorType(BindingConstraint::opEquality);

    double rhsValue = 50.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    unsigned int nbDaysInWeek = 7;
    BOOST_TEST(output.flow(link).week(0) == rhsValue * nbDaysInWeek, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(daily_BC_restricts_link_direct_capacity_to_60, StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opEquality);

    double rhsValue = 60.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).day(0) == rhsValue, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(Hourly_BC_restricts_link_direct_capacity_to_less_than_90, StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);

    double rhsValue = 90.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).hour(100) <= rhsValue, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(Daily_BC_restricts_link_direct_capacity_to_greater_than_80,
                        StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opGreater);

    double rhsValue = 80.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).hour(100) >= rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_ON_A_CLUSTER)

BOOST_FIXTURE_TEST_CASE(Hourly_BC_restricts_cluster_generation_to_90, StudyWithBConLink)
{
    setNumberMCyears(1);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    double rhsValue = 90.;
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, rhsValue);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.thermalGeneration(cluster.get()).hour(10) == rhsValue, tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TESTING_BC_RHS_SCENARIZATION_WHEN_BC_ON_A_LINK)

BOOST_FIXTURE_TEST_CASE(On_year_2__RHS_TS_number_2_is_taken_into_account, StudyWithBConLink)
{
    setNumberMCyears(2);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    double bcGroupRHS1 = 90.;
    double bcGroupRHS2 = 70.;
    TimeSeriesConfigurer(BC->RHSTimeSeries())
      .setColumnCount(2)
      .fillColumnWith(0, bcGroupRHS1)
      .fillColumnWith(1, bcGroupRHS2);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 0, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 1, 2);

    simulation->create();
    playOnlyYear(1);
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).hour(0) == bcGroupRHS2, tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(On_year_9__RHS_TS_number_4_is_taken_into_account, StudyWithBConLink)
{
    setNumberMCyears(10);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfigurer(BC->RHSTimeSeries())
      .setColumnCount(7)
      .fillColumnWith(0, 10.)
      .fillColumnWith(1, 20.)
      .fillColumnWith(2, 30.)
      .fillColumnWith(3, 40.)
      .fillColumnWith(4, 50.)
      .fillColumnWith(5, 60.)
      .fillColumnWith(6, 70.);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 0, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 1, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 2, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 3, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 4, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 5, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 6, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 7, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 8, 4); // Here year 9
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 9, 1);

    simulation->create();
    playOnlyYear(8);
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).hour(0) == 40., tt::tolerance(0.001));
}

BOOST_FIXTURE_TEST_CASE(On_year_9__RHS_TS_number_4_out_of_bound_use_random_fallback_to_Oth_column,
                        StudyWithBConLink)
{
    setNumberMCyears(10);

    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opEquality);

    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, 0.);

    ScenarioBuilderRule scenarioBuilderRule(*study);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 0, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 1, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 2, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 3, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 4, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 5, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 6, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 7, 1);
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 8, 42); // Here year 9
    scenarioBuilderRule.bcGroup().setTSnumber(BC->group(), 9, 1);

    simulation->create();
    playOnlyYear(8);
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    BOOST_TEST(output.flow(link).hour(0) == 0., tt::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
