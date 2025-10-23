/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#define BOOST_TEST_MODULE test_api
#define WIN32_LEAN_AND_MEAN

#include <memory>

#include <boost/test/unit_test.hpp>

#include "antares/api/singleProblemGetter.h"
#include "antares/study/study.h"

#include "in-memory-study.h"

BOOST_AUTO_TEST_CASE(single_problem_nominal_case)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);
    builder.setNumberMCyears(2);
    auto* area = builder.addAreaToStudy("AREA");
    auto thCluster = addClusterToArea(area, "dispatch-cluster");
    ThermalClusterConfig clusterConfig(thCluster);
    clusterConfig.setNominalCapacity(100)
      .setUnitCount(1)
      .setCosts(20.)
      .setAvailablePowerNumberOfTS(1)
      .setAvailablePower(0, 102.);
    addScratchpadToEachArea(*builder.study); // TODO StudyBuilder should have a `run` method that
                                             // calls this method and returns a study
    Antares::Solver::SingleProblemGetter getter(std::move(builder.study));
    const Antares::Solver::ConstantDataFromAntares constantData = getter.getConstantData();
    const Antares::Solver::WeeklyDataFromAntares fistWeekData = getter.getWeeklyData({0, 0});
}
