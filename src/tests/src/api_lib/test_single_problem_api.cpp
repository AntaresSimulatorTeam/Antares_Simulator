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

std::size_t findIndex(const std::vector<std::string>& v, const std::string& value)
{
    std::size_t ret = std::distance(v.begin(), std::find(v.begin(), v.end(), value));
    BOOST_REQUIRE(ret < v.size());
    return ret;
}

BOOST_AUTO_TEST_CASE(single_problem_nominal_case)
{
    StudyBuilder builder;
    builder.simulationBetweenDays(0, 14);
    builder.setNumberMCyears(2);
    auto* area = builder.addAreaToStudy("AREA");
    area->hydro.reservoirManagement = false;
    auto thCluster = addClusterToArea(area, "dispatch-cluster");
    ThermalClusterConfig clusterConfig(thCluster);
    clusterConfig.setNominalCapacity(100)
      .setUnitCount(1)
      .setCosts(20.)
      .setAvailablePowerNumberOfTS(1)
      .setAvailablePower(0, 102.);
    // TODO StudyBuilder should have a `run` method that
    // calls addScratchpadToEachArea and initializeRuntimeInfos
    // auto study = builder.run();
    builder.study->initializeRuntimeInfos();
    addScratchpadToEachArea(*builder.study);

    // TODO this is HORRIBLE
    // more specifically, this resize is usually done when loading from files. It's all good, except
    // when you DON'T LOAD FILES.
    area->hydro.deltaBetweenFinalAndInitialLevels.resize(builder.study->parameters.nbYears);

    Antares::Solver::SingleProblemGetter getter(std::move(builder.study));
    const Antares::Solver::ConstantDataFromAntares constantData = getter.getConstantData();
    // 504 = 3*168, 3 sets of variables
    // unsupplied energy
    // spilled energy
    // dispatchable production
    BOOST_CHECK_EQUAL(constantData.VariablesCount, 504);
    // 336 = 2*168
    // area balance
    // fictive loads
    BOOST_CHECK_EQUAL(constantData.ConstraintesCount, 336);

    const auto dispatchableVariable = findIndex(
      constantData.VariablesMeaning,
      "DispatchableProduction::area<area>::ThermalCluster<dispatch-cluster>::hour<0>");
    const auto unsuppliedVariable = findIndex(constantData.VariablesMeaning,
                                              "PositiveUnsuppliedEnergy::area<area>::hour<0>");
    const auto spilledVariable = findIndex(constantData.VariablesMeaning,
                                           "NegativeUnsuppliedEnergy::area<area>::hour<0>");

    const auto areaBalanceConstraint = findIndex(constantData.ConstraintsMeaning,
                                                 "AreaBalance::area<area>::hour<0>");

    // TODO explain
    BOOST_CHECK_EQUAL(constantData.ColumnIndexes[0], 0);
    BOOST_CHECK_EQUAL(constantData.ColumnIndexes[1], 1);
    BOOST_CHECK_EQUAL(constantData.ConstraintsMatrixCoeff[0], -1);
    BOOST_CHECK_EQUAL(constantData.ConstraintsMatrixCoeff[1], -1);

    const Antares::Solver::WeeklyDataFromAntares firstWeekData = getter.getWeeklyData({0, 0});
    // COST
    BOOST_CHECK_EQUAL(firstWeekData.LinearCost[dispatchableVariable],
                      20.717999329259154); // thermal cost 20 + noise
    BOOST_CHECK_EQUAL(firstWeekData.LinearCost[unsuppliedVariable], 1000.); // unsupplied cost
    BOOST_CHECK_EQUAL(firstWeekData.LinearCost[spilledVariable], 1000.);    // spilled cost
    // BOUNDS
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[dispatchableVariable], 0.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[unsuppliedVariable], 0.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmin[spilledVariable], 0.);

    BOOST_CHECK_EQUAL(firstWeekData.Xmax[dispatchableVariable], 102.);
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[unsuppliedVariable],
                      1.e-5); // default value when there is no residual load
    BOOST_CHECK_EQUAL(firstWeekData.Xmax[spilledVariable], 1.e80);
}
