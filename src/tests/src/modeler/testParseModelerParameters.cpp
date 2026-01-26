// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <yaml-cpp/yaml.h>

#include <antares/solver/modeler/parameters/parseModelerParameters.h>

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver::LoadFiles;

BOOST_AUTO_TEST_SUITE(parse_modeler_parameters_tests)

BOOST_AUTO_TEST_CASE(parse_basic_parameters)
{
    std::string yamlContent = R"(
solver: SCIP
solver-logs: false
no-output: true
export-mps: false
first-time-step: 0
last-time-step: 23
)";

    ModelerParameters params = parseModelerParameters(yamlContent);

    BOOST_CHECK_EQUAL(params.solver, "SCIP");
    BOOST_CHECK_EQUAL(params.solverLogs, false);
    BOOST_CHECK_EQUAL(params.noOutput, true);
    BOOST_CHECK_EQUAL(params.exportMps, false);
    BOOST_CHECK_EQUAL(params.firstTimeStep, 0);
    BOOST_CHECK_EQUAL(params.lastTimeStep, 23);
}

BOOST_AUTO_TEST_CASE(parse_parameters_with_defaults)
{
    std::string yamlContent = R"(
solver: GLOP
first-time-step: 1
last-time-step: 10
)";

    ModelerParameters params = parseModelerParameters(yamlContent);

    BOOST_CHECK_EQUAL(params.solver, "GLOP");
    BOOST_CHECK_EQUAL(params.firstTimeStep, 1);
    BOOST_CHECK_EQUAL(params.lastTimeStep, 10);
}

BOOST_AUTO_TEST_CASE(parse_parameters_throws_on_invalid_yaml)
{
    std::string invalidYaml = R"(
this is not: [valid yaml
)";

    BOOST_CHECK_THROW(parseModelerParameters(invalidYaml), YAML::Exception);
}

BOOST_AUTO_TEST_CASE(parse_parameters_with_export_mps_enabled)
{
    std::string yamlContent = R"(
solver: SCIP
export-mps: true
first-time-step: 0
last-time-step: 5
)";

    ModelerParameters params = parseModelerParameters(yamlContent);

    BOOST_CHECK_EQUAL(params.exportMps, true);
    BOOST_CHECK_EQUAL(params.solver, "SCIP");
}

BOOST_AUTO_TEST_CASE(parse_parameters_with_solver_logs_enabled)
{
    std::string yamlContent = R"(
solver: SCIP
solver-logs: true
first-time-step: 0
last-time-step: 5
)";

    ModelerParameters params = parseModelerParameters(yamlContent);

    BOOST_CHECK_EQUAL(params.solverLogs, true);
}

BOOST_AUTO_TEST_CASE(parse_empty_yaml_throws)
{
    std::string emptyYaml = "";

    // Empty YAML should result in missing required fields.
    BOOST_CHECK_THROW(parseModelerParameters(emptyYaml), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
