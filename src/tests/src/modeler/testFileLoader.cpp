// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <filesystem>

#include <antares/solver/modeler/loadFiles/Fileloader.h>
#include <antares/solver/modeler/ModelerData.h>
#include <antares/solver/modeler/parameters/modelerParameters.h>

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver;
using namespace Antares::Solver::LoadFiles;

BOOST_AUTO_TEST_SUITE(file_loader_tests)

BOOST_AUTO_TEST_CASE(file_loader_throws_on_nonexistent_path)
{
    std::filesystem::path nonexistentPath = "/nonexistent/path/to/study";
    FileLoader loader(nonexistentPath);

    // Loading from a nonexistent path should throw.
    BOOST_CHECK_THROW(loader.loadParameters(), std::exception);
}

BOOST_AUTO_TEST_CASE(file_loader_throws_on_invalid_study)
{
    // Create a temporary directory without valid study files.
    auto tempDir = std::filesystem::temp_directory_path() / "antares_test_invalid_study";
    std::filesystem::create_directories(tempDir);

    FileLoader loader(tempDir);

    // Loading from an invalid study should throw.
    BOOST_CHECK_THROW(loader.loadParameters(), std::exception);

    // Cleanup.
    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_SUITE_END()
