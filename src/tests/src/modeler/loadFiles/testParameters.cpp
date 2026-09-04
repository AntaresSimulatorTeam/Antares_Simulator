// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <fstream>

#include <boost/test/unit_test.hpp>

#include <antares/solver/modeler/loadFiles/loadFiles.h>
#include <antares/solver/modeler/parameters/scenarioScope.h>

#include "files-system.h"

BOOST_AUTO_TEST_CASE(read_parameters)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver: sirius
        solver-logs: false
        solver-parameters: PRESOLVE 1
        no-output: true
        export-mps: true
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
    BOOST_CHECK_EQUAL(params.exportMps, true);
}

BOOST_AUTO_TEST_CASE(read_parameters_out_of_order)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver-logs: false
        solver: sirius
        solver-parameters: PRESOLVE 1
        no-output: true
        export-mps: false
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_CHECK_EQUAL(params.solver, "sirius");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.solverParameters, "PRESOLVE 1");
    BOOST_CHECK_EQUAL(params.noOutput, true);
    BOOST_CHECK_EQUAL(params.exportMps, false);
}

BOOST_AUTO_TEST_CASE(parameters_missing)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver-logs: false
        no-output: true
    )";
    paramStream.close();

    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadParameters(studyPath), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(file_missing)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    BOOST_CHECK_THROW(Antares::Solver::LoadFiles::loadParameters(studyPath), std::runtime_error);
}

// --- scenario-scope parsing ---

BOOST_AUTO_TEST_CASE(parse_scenario_scope_inline)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver: sirius
        solver-parameters: PRESOLVE 1
        scenario-scope:
          include:
            - 5
            - "3"
            - "0-9"
          exclude:
            - 9
            - 14
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_REQUIRE_EQUAL(params.scenarioScope.include.size(), 3);
    BOOST_CHECK_EQUAL(params.scenarioScope.include[0], "5");
    BOOST_CHECK_EQUAL(params.scenarioScope.include[1], "3");
    BOOST_CHECK_EQUAL(params.scenarioScope.include[2], "0-9");
    BOOST_REQUIRE_EQUAL(params.scenarioScope.exclude.size(), 2);
    BOOST_CHECK_EQUAL(params.scenarioScope.exclude[0], "9");
    BOOST_CHECK_EQUAL(params.scenarioScope.exclude[1], "14");
    BOOST_CHECK(!params.scenarioScope.playlistFile.has_value());
}

BOOST_AUTO_TEST_CASE(parse_scenario_scope_playlist_file)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver: sirius
        solver-parameters: PRESOLVE 1
        scenario-scope:
          playlist-file: my-playlist.json
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    BOOST_REQUIRE(params.scenarioScope.playlistFile.has_value());
    BOOST_CHECK_EQUAL(params.scenarioScope.playlistFile->string(), "my-playlist.json");
    BOOST_CHECK(params.scenarioScope.include.empty());
}

BOOST_AUTO_TEST_CASE(parse_scenario_scope_empty_block_is_default)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream paramStream(studyPath / "parameters.yml");
    paramStream << R"(
        solver: sirius
        solver-parameters: PRESOLVE 1
        scenario-scope:
    )";
    paramStream.close();

    auto params = Antares::Solver::LoadFiles::loadParameters(studyPath);
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(params.scenarioScope,
                                                                    studyPath);
    BOOST_REQUIRE_EQUAL(scenarios.size(), 1);
    BOOST_CHECK_EQUAL(scenarios[0], 0);
}

// --- scenario-scope resolution ---

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_default_runs_scenario_0)
{
    Antares::Solver::ScenarioScope scope; // empty block
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    BOOST_REQUIRE_EQUAL(scenarios.size(), 1);
    BOOST_CHECK_EQUAL(scenarios[0], 0);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_single)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"0"};
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    BOOST_REQUIRE_EQUAL(scenarios.size(), 1);
    BOOST_CHECK_EQUAL(scenarios[0], 0);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_range)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"0-99"};
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    BOOST_REQUIRE_EQUAL(scenarios.size(), 100);
    BOOST_CHECK_EQUAL(scenarios.front(), 0);
    BOOST_CHECK_EQUAL(scenarios.back(), 99);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_dedup_and_sort)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"0-19", "49-59"};
    scope.exclude = {"9", "14"};
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});

    // 0-19 (20) + 49-59 (11) - {9,14} (2) = 29
    BOOST_REQUIRE_EQUAL(scenarios.size(), 29);
    // deduplicated and sorted
    BOOST_CHECK_EQUAL(scenarios.front(), 0);
    BOOST_CHECK_EQUAL(scenarios.back(), 59);
    // excluded scenarios are removed
    BOOST_CHECK(std::find(scenarios.begin(), scenarios.end(), 9) == scenarios.end());
    BOOST_CHECK(std::find(scenarios.begin(), scenarios.end(), 14) == scenarios.end());
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_overlap_deduplicated)
{
    Antares::Solver::ScenarioScope scope;
    // overlapping ranges: 0-9 and 5-14 -> 0..14 (15 scenarios)
    scope.include = {"0-9", "5-14"};
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    BOOST_REQUIRE_EQUAL(scenarios.size(), 15);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_exclude_not_in_base_warns_no_effect)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"0-4"};
    scope.exclude = {"99"}; // not present -> no effect
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    BOOST_REQUIRE_EQUAL(scenarios.size(), 5);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_exclude_without_include_throws)
{
    Antares::Solver::ScenarioScope scope;
    scope.exclude = {"0"};
    BOOST_CHECK_THROW(Antares::Solver::resolveScenarioScopeScenarios(scope, {}),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_negative_index_throws)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"-1"};
    BOOST_CHECK_THROW(Antares::Solver::resolveScenarioScopeScenarios(scope, {}),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_include_and_playlist_mutually_exclusive)
{
    Antares::Solver::ScenarioScope scope;
    scope.include = {"0"};
    scope.playlistFile = "playlist.json";
    BOOST_CHECK_THROW(Antares::Solver::resolveScenarioScopeScenarios(scope, {}),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_playlist_file)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream playlistStream(studyPath / "playlist.json");
    playlistStream << R"([0, "1", "3-6"])";
    playlistStream.close();

    Antares::Solver::ScenarioScope scope;
    scope.playlistFile = "playlist.json";
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, studyPath);
    // 0, 1, 3, 4, 5, 6
    BOOST_REQUIRE_EQUAL(scenarios.size(), 6);
    BOOST_CHECK_EQUAL(scenarios[0], 0);
    BOOST_CHECK_EQUAL(scenarios[1], 1);
    BOOST_CHECK_EQUAL(scenarios[2], 3);
    BOOST_CHECK_EQUAL(scenarios[5], 6);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_playlist_file_with_exclude)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream playlistStream(studyPath / "playlist.json");
    playlistStream << R"([0, 1, 2, 3])";
    playlistStream.close();

    Antares::Solver::ScenarioScope scope;
    scope.playlistFile = "playlist.json";
    scope.exclude = {"1"};
    auto scenarios = Antares::Solver::resolveScenarioScopeScenarios(scope, studyPath);
    BOOST_REQUIRE_EQUAL(scenarios.size(), 3);
    BOOST_CHECK_EQUAL(scenarios[0], 0);
    BOOST_CHECK_EQUAL(scenarios[1], 2);
    BOOST_CHECK_EQUAL(scenarios[2], 3);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_negative_index_reports_sign_message)
{
    // "-1" must be reported as a negative index (not a generic format error).
    Antares::Solver::ScenarioScope scope;
    scope.include = {"-1"};
    bool caught = false;
    std::string message;
    try
    {
        Antares::Solver::resolveScenarioScopeScenarios(scope, {});
    }
    catch (const std::invalid_argument& e)
    {
        caught = true;
        message = e.what();
    }
    BOOST_CHECK(caught);
    BOOST_CHECK(message.find("indices must be >= 0") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_plus_sign_is_rejected)
{
    // A leading '+' is not part of the documented grammar and must be rejected.
    Antares::Solver::ScenarioScope scope;
    scope.include = {"+5"};
    BOOST_CHECK_THROW(Antares::Solver::resolveScenarioScopeScenarios(scope, {}),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(resolve_scenario_scope_playlist_broken_json_reports_path)
{
    auto studyPath = CREATE_TMP_DIR_BASED_ON_TEST_NAME();
    std::ofstream playlistStream(studyPath / "playlist.json");
    playlistStream << R"([0, 1,)"; // invalid JSON
    playlistStream.close();

    Antares::Solver::ScenarioScope scope;
    scope.playlistFile = "playlist.json";
    bool caught = false;
    std::string message;
    try
    {
        Antares::Solver::resolveScenarioScopeScenarios(scope, studyPath);
    }
    catch (const std::invalid_argument& e)
    {
        caught = true;
        message = e.what();
    }
    BOOST_CHECK(caught);
    // The file path must be part of the message so the broken playlist can be located.
    BOOST_CHECK(message.find("playlist.json") != std::string::npos);
}
