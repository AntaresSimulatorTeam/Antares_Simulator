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

#define BOOST_TEST_MODULE test_api
#define WIN32_LEAN_AND_MEAN

#include <filesystem>

#include <boost/test/unit_test.hpp>

#include <antares/antares/fatal-error.h>

#include "../../src/utils/unit_test_utils.h"
#include "API.h"
#include "in-memory-study.h"

class InMemoryStudyLoader: public Antares::IStudyLoader
{
public:
    explicit InMemoryStudyLoader(bool success = true):
        success_(success)
    {
    }

    [[nodiscard]] std::unique_ptr<Antares::Data::Study> load() const override
    {
        if (!success_)
        {
            return nullptr;
        }
        StudyBuilder builder;
        builder.addAreaToStudy("area1");
        builder.addAreaToStudy("area2");
        builder.study->initializeRuntimeInfos();
        builder.setNumberMCyears(1);
        builder.study->parameters.resultFormat = ResultFormat::inMemory;
        builder.study->prepareOutput();
        return std::move(builder.study);
    }

    bool success_ = true;
};

BOOST_AUTO_TEST_CASE(simulation_path_points_to_results)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>();
    auto results = api.run(*study_loader.get(), {});
    BOOST_CHECK_EQUAL(results.simulationPath, std::filesystem::path{"no_output"});
    // Testing for "no_output" is a bit weird, but it's the only way to test this without actually
    // running the simulation
}

BOOST_AUTO_TEST_CASE(api_run_contains_antares_problem)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>();
    auto results = api.run(*study_loader.get(), {});

    BOOST_CHECK(!results.antares_problems.empty());
    BOOST_CHECK(!results.error);
}

BOOST_AUTO_TEST_CASE(result_failure_when_study_is_null)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>(false);
    auto results = api.run(*study_loader.get(), {});

    BOOST_CHECK(results.error);
}

// Test where data in problems are consistant with data in study
BOOST_AUTO_TEST_CASE(result_contains_problems)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>();
    auto results = api.run(*study_loader.get(), {});

    BOOST_CHECK(!results.antares_problems.empty());
    BOOST_CHECK(!results.error);
    BOOST_CHECK_EQUAL(results.antares_problems.weeklyProblems.size(), 52);
}

// Test where data in problems are consistant with data in study
BOOST_AUTO_TEST_CASE(result_with_ortools_coin)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>();
    const Antares::Solver::Optimization::OptimizationOptions opt{.ortoolsUsed = true,
                                                                 .ortoolsSolver = "coin",
                                                                 .solverLogs = false,
                                                                 .solverParameters = ""};

    auto results = api.run(*study_loader.get(), opt);

    BOOST_CHECK(!results.antares_problems.empty());
    BOOST_CHECK(!results.error);
    BOOST_CHECK_EQUAL(results.antares_problems.weeklyProblems.size(), 52);
}

// Test where we use an invalid OR-Tools solver
BOOST_AUTO_TEST_CASE(invalid_ortools_solver)
{
    Antares::API::APIInternal api;
    auto study_loader = std::make_unique<InMemoryStudyLoader>();
    const Antares::Solver::Optimization::OptimizationOptions opt{
      .ortoolsUsed = true,
      .ortoolsSolver = "this-solver-does-not-exist",
      .solverLogs = true,
      .solverParameters = ""};

    auto shouldThrow = [&api, &study_loader, &opt] { return api.run(*study_loader.get(), opt); };
    BOOST_CHECK_EXCEPTION(shouldThrow(),
                          std::invalid_argument,
                          checkMessage("Solver this-solver-does-not-exist not found"));
}
