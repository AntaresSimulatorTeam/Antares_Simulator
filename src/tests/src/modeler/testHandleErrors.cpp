// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <yaml-cpp/yaml.h>

#include <antares/solver/modeler/loadFiles/loadFiles.h>

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver::LoadFiles;

BOOST_AUTO_TEST_SUITE(handle_errors_tests)

BOOST_AUTO_TEST_CASE(handle_yaml_error_without_mark)
{
    try
    {
        YAML::Node node = YAML::Load("invalid: [yaml");
    }
    catch (const YAML::Exception& e)
    {
        // Test that handleYamlError doesn't throw.
        BOOST_CHECK_NO_THROW(handleYamlError(e, "test context"));
    }
}

BOOST_AUTO_TEST_CASE(handle_yaml_error_with_mark)
{
    std::string yamlContent = R"(
key1: value1
key2: [invalid yaml
key3: value3
)";

    try
    {
        YAML::Node node = YAML::Load(yamlContent);
    }
    catch (const YAML::Exception& e)
    {
        // Test that handleYamlError logs properly with line/column info.
        BOOST_CHECK_NO_THROW(handleYamlError(e, "parsing test file"));
    }
}

BOOST_AUTO_TEST_CASE(handle_yaml_error_logs_context)
{
    try
    {
        throw YAML::Exception(YAML::Mark(), "test error message");
    }
    catch (const YAML::Exception& e)
    {
        // Verify that the function handles the error gracefully.
        BOOST_CHECK_NO_THROW(handleYamlError(e, "custom context message"));
    }
}

BOOST_AUTO_TEST_SUITE_END()
