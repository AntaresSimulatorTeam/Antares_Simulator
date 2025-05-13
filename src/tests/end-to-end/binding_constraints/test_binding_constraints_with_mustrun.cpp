/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <span>

#include <boost/test/unit_test.hpp>

#include "in-memory-study.h"

namespace tt = boost::test_tools;

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

    TimeSeriesConfigurer(area->load.series.timeSeries).setColumnCount(1).fillColumnWith(0, 1000.);

    // Adding a dispatchable cluster to the previous area
    cluster_dispatch = addClusterToArea(area, "dispatch-cluster");
    ThermalClusterConfig(cluster_dispatch.get())
      .setNominalCapacity(1000.)
      .setAvailablePower(0, 1000.)
      .setCosts(50.)
      .setUnitCount(1);

    // Adding a mustrun cluster to the previous area
    cluster_mustrun = addClusterToArea(area, "mustrun-cluster");
    cluster_mustrun->mustrun = true;
    ThermalClusterConfig(cluster_mustrun.get())
      .setNominalCapacity(100.)
      .setAvailablePower(0, 100.)
      .setCosts(10.)
      .setUnitCount(1);

    setNumberMCyears(1); // Can cause a crash if moved up
}

BOOST_AUTO_TEST_SUITE(TESTS_BINDING_CONSTRAINTS_WITH_MUSTRUN_CLUSTERS)

BOOST_FIXTURE_TEST_CASE(very_simple_hourly_BC_restricts_dispatchable_production_to_900,
                        StudyWithTwoClusters)
{
    // Creating the binding constraint :
    // ===============================
    // cluster_disp + cluster_mustrun < 1000 <==> ... <==> cluster_disp < 900
    auto BC = addBindingConstraints(*study, "some BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, 1000.);
    BC->weight(cluster_dispatch.get(), 1);
    BC->weight(cluster_mustrun.get(), 1);
    BC->enabled(true);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    auto dispatch_prod = std::span<double>{output.thermalGeneration(cluster_dispatch.get()).hours(),
                                           Constants::nbHoursInAWeek};
    std::vector<double> expected_values(Constants::nbHoursInAWeek, 900.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values));
}

BOOST_FIXTURE_TEST_CASE(hourly_BC_restricts_dispatchable_production_to_450, StudyWithTwoClusters)
{
    // Creating the binding constraint :
    // ===============================
    // 2 * cluster_disp + 3 * cluster_mustrun < 1200 <==> ... <==> cluster_disp < 450
    auto BC = addBindingConstraints(*study, "some BC", "some scenario group");
    BC->setTimeGranularity(BindingConstraint::typeHourly);
    BC->operatorType(BindingConstraint::opLess);
    TimeSeriesConfigurer(BC->RHSTimeSeries()).setColumnCount(1).fillColumnWith(0, 1200.);
    BC->weight(cluster_dispatch.get(), 2);
    BC->weight(cluster_mustrun.get(), 3);
    BC->enabled(true);

    simulation->create();
    simulation->run();

    OutputRetriever output(simulation->rawSimu());
    auto dispatch_prod = std::span<double>{output.thermalGeneration(cluster_dispatch.get()).hours(),
                                           Constants::nbHoursInAWeek};
    std::vector<double> expected_values(Constants::nbHoursInAWeek, 450.);
    BOOST_TEST(std::ranges::equal(dispatch_prod, expected_values));
}

BOOST_AUTO_TEST_SUITE_END()
