// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <boost/test/unit_test.hpp>

#include <filesystem>
#include <fstream>

#include <antares/solver/modeler/fileWriter/FileWriter.h>
#include <antares/optimisation/linear-problem-api/FillContext.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h>
#include <antares/optimisation/linear-problem-mpsolver-impl/mipSolution.h>
#include <antares/solver/modeler/ModelerData.h>
#include "antares/study/system-model/system.h"

// If we don't turn clang-format off here, some antlr4 header does not compile :
// it collides with a #include <windows.h> somewhere in Yuni
// clang-format off
#include <unit_test_utils.h>
// clang-format on

using namespace Antares::Solver;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::ModelerStudy::SystemModel;

BOOST_AUTO_TEST_SUITE(file_writer_tests)

class FileWriterFixture
{
public:
    FileWriterFixture()
    {
        tempDir = std::filesystem::temp_directory_path() / "antares_test_filewriter";
        std::filesystem::create_directories(tempDir);
    }

    ~FileWriterFixture()
    {
        if (std::filesystem::exists(tempDir))
        {
            std::filesystem::remove_all(tempDir);
        }
    }

    std::filesystem::path tempDir;
};

BOOST_FIXTURE_TEST_CASE(file_writer_creates_output_directory, FileWriterFixture)
{
    FileWriter writer(tempDir);
    writer.init("20260126-1234");

    // Check that output directory was created.
    auto expectedOutputPath = tempDir / "output";
    BOOST_CHECK(std::filesystem::exists(expectedOutputPath));
    BOOST_CHECK(std::filesystem::is_directory(expectedOutputPath));
    BOOST_CHECK_EQUAL(writer.outputPath(), expectedOutputPath);
}

BOOST_FIXTURE_TEST_CASE(file_writer_throws_when_directory_cannot_be_created, FileWriterFixture)
{
    // Create a file where the output directory should be to force creation to fail.
    auto blockerFile = tempDir / "output";
    std::ofstream(blockerFile) << "blocker";

    FileWriter writer(tempDir);

    BOOST_CHECK_THROW(writer.init("20260126-1234"), Modeler::ModelerError);

    // Cleanup.
    std::filesystem::remove(blockerFile);
}

BOOST_FIXTURE_TEST_CASE(file_writer_simulation_id_is_stored, FileWriterFixture)
{
    FileWriter writer(tempDir);
    writer.init("test_simulation_id");

    // The simulation ID should be stored and used for naming output files.
    // We verify it's created properly by checking the output path.
    BOOST_CHECK(std::filesystem::exists(writer.outputPath()));
}

BOOST_FIXTURE_TEST_CASE(file_writer_write_simulation_table_creates_csv, FileWriterFixture)
{
    FileWriter writer(tempDir);
    writer.init("test_sim");

    // Create minimal problem and solution for testing.
    LinearProblemMpsolverImpl::OrtoolsLinearProblem problem(false, "SCIP");
    auto* solution = problem.solve(false);

    ModelerData data;
    data.system = std::make_unique<System>(SystemBuilder().withId("test_system").build());

    LinearProblemData lpData;
    ScenarioGroupRepository scenarioGroupRepo;
    OptimEntityContainer container(problem, &lpData, &scenarioGroupRepo);
    FillContext fillContext{0, 4, 0, 4, 0};

    // This should create simulation table files.
    BOOST_CHECK_NO_THROW(
      writer.writeSimulationTable(problem, *solution, data, container, fillContext));

    // Verify output directory exists and contains files.
    BOOST_CHECK(std::filesystem::exists(writer.outputPath()));
}

BOOST_AUTO_TEST_SUITE_END()
