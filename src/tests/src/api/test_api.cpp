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

#define BOOST_TEST_MODULE test api
#define BOOST_TEST_DYN_LINK

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>
#include <filesystem>
#include "antares/api/solver.h"
#include "antares/api/SimulationResults.h"

BOOST_AUTO_TEST_CASE(simulation_path_points_to_results)
{
    const Antares::API::SimulationResults results{
      Antares::API::PerformSimulation(std::filesystem::path())};
    BOOST_CHECK(!results.simulationPath.empty());
}

BOOST_AUTO_TEST_CASE(simulation_path_points_to_valid_output)
{
    using namespace std::string_literals;
    const Antares::API::SimulationResults results{
      Antares::API::PerformSimulation(std::filesystem::path())};
    BOOST_CHECK_EQUAL(results.simulationPath, std::filesystem::path{"output_path"s});
}