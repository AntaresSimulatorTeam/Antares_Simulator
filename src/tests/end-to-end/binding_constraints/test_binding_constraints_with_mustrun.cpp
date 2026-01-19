// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <algorithm> // std::ranges::equal(...)
#include <span>

#include <boost/test/unit_test.hpp>

#include "in-memory-study.h"

struct StudyWithTwoClusters: public StudyBuilder
{
    using StudyBuilder::StudyBuilder;
    StudyWithTwoClusters();

    // Data members
    std::shared_ptr<ThermalCluster> cluster_dispatch;
    std::shared_ptr<ThermalCluster> cluster_mustrun;
};

StudyWithTwoClusters::StudyWithTwoClusters()
{
    simulationBetweenDays(0, 7);

    Area* area = addAreaToStudy("some area");

    TimeSeriesConfigurer(area->load.series.timeSeries).setDimensions(1).fillColumnWith(0, 1000.);

    // Adding a dispatchable cluster to the previous area
    cluster_dispatch = addClusterToArea(area, "dispatch-cluster");
    ThermalClusterConfig(cluster_dispatch)
      .setNominalCapacity(1000.)
      .setAvailablePower(0, 1000.)
      .setCosts(50.)
      .setUnitCount(1);

    // Adding a mustrun cluster to the previous area
    cluster_mustrun = addClusterToArea(area, "mustrun-cluster");
    cluster_mustrun->mustrun = true;
    ThermalClusterConfig(cluster_mustrun)
      .setNominalCapacity(100.)
      .setAvailablePower(0, 100.)
      .setCosts(10.)
      .setUnitCount(1);

    setNumberMCyears(1); // Can cause a crash if moved up
}

BOOST_AUTO_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_WITH_MUSTRUN_CLUSTERS)

BOOST_FIXTURE_TEST_CASE(in_hourly_BC__weights_are_1__it_restricts_dispatchable_production,
                        StudyWithTwoClusters)
{
    // Creating the hourly BC :
    // ======================
    // cluster_disp + cluster_mustrun < 1000 <==> ... <==> cluster_disp < 900
    auto BC = addBindingConstraints(*study, "some hourly BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setDimensions(1).fillColumnWith(0, 1000.);
    BC->weight(cluster_dispatch.get(), 1);
    BC->weight(cluster_mustrun.get(), 1);
    BC->enabled(true);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    auto dispatch_prod = std::span<long double>{
      output.thermalGeneration(cluster_dispatch.get()).hours(),
      Constants::nbHoursInAWeek};
    std::vector<long double> expected_values(Constants::nbHoursInAWeek, 900.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values));
}

BOOST_FIXTURE_TEST_CASE(in_hourly_BC__weights_are_2_and_3__it_restricts_dispatchable_production,
                        StudyWithTwoClusters)
{
    // Creating the hourly BC :
    // ======================
    // 2 * cluster_disp + 3 * cluster_mustrun < 1200 <==> ... <==> cluster_disp < 450
    auto BC = addBindingConstraints(*study, "some hourly BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setDimensions(1).fillColumnWith(0, 1200.);
    BC->weight(cluster_dispatch.get(), 2);
    BC->weight(cluster_mustrun.get(), 3);
    BC->enabled(true);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    auto dispatch_prod = std::span<long double>{
      output.thermalGeneration(cluster_dispatch.get()).hours(),
      Constants::nbHoursInAWeek};
    std::vector<long double> expected_values(Constants::nbHoursInAWeek, 450.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values));
}

BOOST_FIXTURE_TEST_CASE(in_daily_BC__weights_are_2_and_3__it_restricts_dispatchable_production,
                        StudyWithTwoClusters)
{
    // Creating the daily BC :
    // =====================
    // 2 * cluster_disp + 3 * cluster_mustrun < 30000
    // As cluster_mustrun generates 2400 MWh a day, we have :
    // cluster_disp < 11400
    auto BC = addBindingConstraints(*study, "some daily BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opLess);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setDimensions(1, 366).fillColumnWith(0, 30000.);
    BC->weight(cluster_dispatch.get(), 2);
    BC->weight(cluster_mustrun.get(), 3);
    BC->enabled(true);

    simulation.create();
    simulation.run();

    OutputRetriever out(simulation.rawSimu());
    auto dispatch_prod = std::span<long double>{
      out.thermalGeneration(cluster_dispatch.get()).days(),
      7};
    std::vector<long double> expected_values(7, 11400.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values));
}

BOOST_FIXTURE_TEST_CASE(simulation_2_weeks_long__daily_BC_RHS_changes_on_2nd_week,
                        StudyWithTwoClusters)
{
    simulationBetweenDays(0, 14); // Siluation is 2 weeks long

    // Creating the hourly BC :
    // ======================
    // - week 1 : cluster_disp + cluster_mustrun < 20000 <==> cluster_disp < 17600
    // - week 2 : cluster_disp + cluster_mustrun < 17000 <==> cluster_disp < 14600
    //
    auto BC = addBindingConstraints(*study, "some hourly BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opLess);
    std::vector<double> rhs(366, 0.);
    std::fill(rhs.begin(), rhs.begin() + 7, 20000.);
    std::fill(rhs.begin() + 7, rhs.begin() + 14, 17000.);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setDimensions(1, 366).fillColumnWith(0, rhs);
    BC->weight(cluster_dispatch.get(), 1);
    BC->weight(cluster_mustrun.get(), 1);
    BC->enabled(true);

    simulation.create();
    simulation.run();

    OutputRetriever out(simulation.rawSimu());
    // Week 1
    auto dispatch_prod = std::span<long double>{
      out.thermalGeneration(cluster_dispatch.get()).days(),
      7};
    std::vector<long double> expected_values_week_1(7, 17600.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values_week_1));

    // Week 2
    dispatch_prod = std::span<long double>{out.thermalGeneration(cluster_dispatch.get()).days() + 7,
                                           7};
    std::vector<long double> expected_values_week_2(7, 14600.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values_week_2));
}

BOOST_FIXTURE_TEST_CASE(in_weekly_BC__weights_are_3_and_4__it_restricts_dispatchable_production,
                        StudyWithTwoClusters)
{
    // Creating the weekly BC :
    // ======================
    // 3 * cluster_disp + 4 * cluster_mustrun < 120000
    // As cluster_mustrun generates 16800 MWh a week, we have :
    // 3 * cluster_disp + 4 * 16800 < 120000
    // 3 * cluster_disp < 120000 - 67200
    // 3 * cluster_disp < 55800
    // cluster_disp < 17600
    auto BC = addBindingConstraints(*study, "some daily BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeDaily);
    BC->operatorType(BindingConstraint::opLess);
    const double dailyRHS = 120000. / 7; // For a weekly BC, RHS is still a daily TS.
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setDimensions(1, 366).fillColumnWith(0, dailyRHS);
    BC->weight(cluster_dispatch.get(), 3);
    BC->weight(cluster_mustrun.get(), 4);
    BC->enabled(true);

    simulation.create();
    simulation.run();

    OutputRetriever output(simulation.rawSimu());
    double dispatch_prod = output.thermalGeneration(cluster_dispatch.get()).week(0);
    BOOST_TEST(dispatch_prod == 17600., boost::test_tools::tolerance(0.001));
}

BOOST_AUTO_TEST_SUITE_END()
