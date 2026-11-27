// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include <antares/exception/InvalidArgumentError.hpp>

// Mock includes for testing - replace with actual includes
#include <inmemory-modeler.h>
#include <unit_test_utils.h>

#include "antares/expressions/visitors/VariabilityVisitor.h"
#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTableEntry.h"
#include "antares/io/outputs/SimulationTableGenerator.h"
#include "antares/io/outputs/SimulationTablePath.h"
#include "antares/io/outputs/SimulationTableStage.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/optimisation/linear-problem-api/linearProblemBuilder.h"
#include "antares/optimisation/linear-problem-data-impl/Scenario.h"
#include "antares/optimisation/linear-problem-data-impl/linearProblemData.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"
#include "antares/optimisation/linear-problem-mpsolver-impl/linearProblem.h"
#include "antares/solver/optim-model-filler/ComponentFiller.h"
#include "antares/solver/optim-model-filler/Dimensions.h"
#include "antares/writer/LegacySimulationTablesWriter.h"
#include "antares/writer/in_memory_writer.h"
#include "antares/writer/simulation_table_writer.h"

#include "UtilMocks.h"

using namespace Antares::LinearProblem::Api;
using namespace Antares::LinearProblem::MpsolverImpl;

using namespace std;
using namespace Antares::Optimization;
using namespace Antares::LinearProblem;
using namespace Antares::LinearProblem::DataImpl;
using namespace Antares::ModelerStudy::SystemModel;
using namespace Antares::IO::Outputs;
using namespace Antares::Writer;
using namespace Antares::Expressions;
using namespace Antares::Expressions::Visitors;

namespace fs = std::filesystem;
namespace rng = std::ranges;

BOOST_AUTO_TEST_SUITE(SupportingMethodsTests)

BOOST_AUTO_TEST_CASE(TestUpdateTimeIndexIfShouldForceScenario)
{
    using TI = VariabilityType;
    // bool = false => no value should change
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::CONSTANT_IN_TIME_AND_SCENARIO, false)
                == TI::CONSTANT_IN_TIME_AND_SCENARIO);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_SCENARIO_ONLY, false)
                == TI::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_TIME_ONLY, false)
                == TI::VARYING_IN_TIME_ONLY);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_TIME_AND_SCENARIO, false)
                == TI::VARYING_IN_TIME_AND_SCENARIO);
    // bool = true => scenario dependency should be added
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::CONSTANT_IN_TIME_AND_SCENARIO, true)
                == TI::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_SCENARIO_ONLY, true)
                == TI::VARYING_IN_SCENARIO_ONLY);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_TIME_ONLY, true)
                == TI::VARYING_IN_TIME_AND_SCENARIO);
    BOOST_CHECK(updateVariabilityIfShouldForceScenario(TI::VARYING_IN_TIME_AND_SCENARIO, true)
                == TI::VARYING_IN_TIME_AND_SCENARIO);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SimulationTableEntryTests)

BOOST_AUTO_TEST_CASE(StatusToString_AllEnumValues)
{
    // Test all enum values
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::FREE), "Free");
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::AT_LOWER_BOUND), "At lower bound");
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::AT_UPPER_BOUND), "At upper bound");
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::FIXED_VALUE), "Fixed value");
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::BASIC), "Basic");
    BOOST_CHECK_EQUAL(StatusToString(MipBasisStatus::NOT_AVAILABLE), "None");
}

BOOST_AUTO_TEST_CASE(StatusToString_EmptyOptional)
{
    std::optional<MipBasisStatus> empty_status;
    BOOST_CHECK_EQUAL(StatusToString(empty_status), "None");
}

BOOST_AUTO_TEST_CASE(StatusToString_ValidOptional)
{
    std::optional<MipBasisStatus> status = MipBasisStatus::BASIC;
    BOOST_CHECK_EQUAL(StatusToString(status), "Basic");
}

BOOST_AUTO_TEST_SUITE_END()

struct SimulationTableFileFixture
{
    SimulationTableFileFixture():
        out_file_path(fs::temp_directory_path() / "simulation-table.csv"),
        simulation_table_writer(out_file_path, TableFormat::CSV)
    {
        remove_if_exists();
    }

    ~SimulationTableFileFixture()
    {
        remove_if_exists();
    }

    fs::path out_file_path;
    SimulationTableWriter simulation_table_writer;

private:
    void remove_if_exists()
    {
        if (fs::exists(out_file_path))
        {
            fs::remove(out_file_path);
        }
    }
};

std::string readFileContent(fs::path& path)
{
    std::ifstream file_istream(path);
    return std::string{std::istreambuf_iterator<char>(file_istream), {}};
}

fs::path makeTemporaryOutputDirectory()
{
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto path = fs::temp_directory_path()
                      / ("antares-simulation-table-output-" + std::to_string(nonce));
    fs::create_directories(path);
    return path;
}

BOOST_AUTO_TEST_SUITE(SimulationTableCsvTests)

BOOST_FIXTURE_TEST_CASE(AddEntry_SingleEntry, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp1",
                               .output = "var1",
                               .absolute_time_index = 100,
                               .block_time_index = 50,
                               .scenario_index = 2,
                               .value = 42.5,
                               .status = MipBasisStatus::BASIC};

    table.addEntry(entry);
    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    // Arrow CSV: header + data row, status is written as a string
    BOOST_CHECK(content.find("block,component,output") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var1,100,50,2,42.5,Basic") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(AddEntry_WithNullOptionals, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 2,
                               .component = "comp2",
                               .output = "var2",
                               .absolute_time_index = std::nullopt,
                               .block_time_index = std::nullopt,
                               .scenario_index = 0,
                               .value = std::nullopt,
                               .status = std::nullopt};

    table.addEntry(entry);
    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    // Arrow CSV: null values are written as "None"
    BOOST_CHECK(content.find("block,component,output") != std::string::npos);
    BOOST_CHECK(content.find("2,comp2,var2,None,None,0,None,None") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(Clear_RemovesAllEntries)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp1",
                               .output = "var1",
                               .absolute_time_index = 100,
                               .block_time_index = 50,
                               .scenario_index = 2,
                               .value = 42.5,
                               .status = MipBasisStatus::BASIC};

    table.addEntry(entry);
    BOOST_CHECK(!table.columns().empty());

    table.clear();

    // After clear
    BOOST_CHECK(table.rowCount() == 0);
}

// The writer flushes its buffer to disk in chunks rather than holding the whole CSV in
// memory. This table is large enough to cross that threshold several times, so a chunk
// boundary landing in the middle of a line would show up as a corrupt or missing row.
BOOST_FIXTURE_TEST_CASE(WriteTable_LargeTableCrossingFlushBoundary, SimulationTableFileFixture)
{
    SimulationTable table;
    const size_t numEntries = 50000;
    for (size_t i = 0; i < numEntries; ++i)
    {
        SimulationTableEntry entry{.block = static_cast<unsigned>(i + 1),
                                   .component = "comp" + std::to_string(i),
                                   .output = "var" + std::to_string(i),
                                   .absolute_time_index = i * 10,
                                   .block_time_index = i % 168,
                                   .scenario_index = static_cast<unsigned>(i % 10),
                                   .value = static_cast<double>(i),
                                   .status = MipBasisStatus::BASIC};
        table.addEntry(entry);
    }

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    // Sanity check: the payload must actually exceed the writer's flush threshold (1 MiB),
    // otherwise this test would silently degrade into a single-chunk case.
    BOOST_CHECK_GT(content.size(), 1u << 20);

    // One header line + one line per entry, and nothing lost or duplicated in between.
    BOOST_CHECK_EQUAL(std::ranges::count(content, '\n'), numEntries + 1);

    // First and last data rows survive intact.
    BOOST_CHECK(content.find("\n1,comp0,var0,0,0,0,0,Basic\n") != std::string::npos);
    BOOST_CHECK(content.find("\n50000,comp49999,var49999,499990,103,9,49999,Basic\n")
                != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FileWriterIntegrationTests)

BOOST_AUTO_TEST_CASE(WriteTo_CreatesCorrectFiles)
{
    OptimisationsSimulationTable tables{nullptr};

    // Add entries to both tables
    SimulationTableEntry entry1{.block = 1,
                                .component = "comp1",
                                .output = "var1",
                                .absolute_time_index = 1,
                                .block_time_index = 1,
                                .scenario_index = 0,
                                .value = 10.0,
                                .status = MipBasisStatus::BASIC};

    SimulationTableEntry entry2{.block = 2,
                                .component = "comp2",
                                .output = "var2",
                                .absolute_time_index = 2,
                                .block_time_index = 2,
                                .scenario_index = 1,
                                .value = 20.0,
                                .status = MipBasisStatus::FREE};

    tables.firstOptimSimulationTable()->addEntry(entry1);
    tables.secondOptimSimulationTable()->addEntry(entry2);

    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    LegacySimulationTablesWriter legacyWriter(tempDir, 1 /* year */, TableFormat::CSV);
    legacyWriter.write(tables);

    // Check that both CSV files were created
    auto file1 = tablesDir / "simulation-table-1-optim-nb-1.csv";
    auto file2 = tablesDir / "simulation-table-1-optim-nb-2.csv";

    BOOST_CHECK(!std::filesystem::exists(tempDir / file1.filename()));
    BOOST_CHECK(!std::filesystem::exists(tempDir / file2.filename()));
    BOOST_CHECK(std::filesystem::is_directory(tablesDir));
    BOOST_CHECK(std::filesystem::exists(file1));
    BOOST_CHECK(std::filesystem::exists(file2));

    // Read and verify content of first file
    // Arrow CSV: status is written as a string
    {
        std::ifstream f(file1);
        std::string content{std::istreambuf_iterator<char>(f), {}};
        BOOST_CHECK(content.find("block,component,output") != std::string::npos);
        BOOST_CHECK(content.find("1,comp1,var1,1,1,0,10,Basic") != std::string::npos);
    }

    // Read and verify content of second file
    {
        std::ifstream f(file2);
        std::string content{std::istreambuf_iterator<char>(f), {}};
        BOOST_CHECK(content.find("block,component,output") != std::string::npos);
        BOOST_CHECK(content.find("2,comp2,var2,2,2,1,20,Free") != std::string::npos);
    }

    // Remove the created files
    std::filesystem::remove_all(tempDir);
}

static void addWeekEntries(SimulationTable& table, unsigned week)
{
    for (unsigned h = 0; h < 3; ++h)
    {
        table.addEntry({.block = week + 1,
                        .component = "comp" + std::to_string(h),
                        .output = "var",
                        .absolute_time_index = week * 168 + h,
                        .block_time_index = h,
                        .scenario_index = 0,
                        .value = 1.5 * (week + h),
                        .status = MipBasisStatus::BASIC});
    }
}

BOOST_AUTO_TEST_CASE(Streaming_CsvFlushPerWeek_IsIdenticalToOneShotWrite)
{
    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    constexpr unsigned nbWeeks = 4;

    // One-shot reference (year 20)
    {
        OptimisationsSimulationTable tables{nullptr};
        for (unsigned w = 0; w < nbWeeks; ++w)
        {
            addWeekEntries(*tables.firstOptimSimulationTable(), w);
        }
        LegacySimulationTablesWriter(tempDir, 20, TableFormat::CSV).write(tables);
    }

    // Streamed: flush after each week, tables are cleared in between (year 21)
    {
        OptimisationsSimulationTable tables{nullptr};
        LegacySimulationTablesWriter writer(tempDir, 21, TableFormat::CSV);
        for (unsigned w = 0; w < nbWeeks; ++w)
        {
            addWeekEntries(*tables.firstOptimSimulationTable(), w);
            writer.flush(tables);
            BOOST_CHECK_EQUAL(tables.firstOptimSimulationTable()->rowCount(), 0u);
        }
        writer.finish(tables);
        // Stage never filled: no file
        BOOST_CHECK(!std::filesystem::exists(tablesDir / "simulation-table-21-optim-nb-2.csv"));
    }

    auto ref = tablesDir / "simulation-table-20-optim-nb-1.csv";
    auto streamed = tablesDir / "simulation-table-21-optim-nb-1.csv";
    BOOST_REQUIRE(std::filesystem::exists(ref));
    BOOST_REQUIRE(std::filesystem::exists(streamed));
    BOOST_CHECK_EQUAL(readFileContent(streamed), readFileContent(ref));
    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_CASE(Streaming_ParquetFlushPerWeek_WritesOneRowGroupPerFlush)
{
    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    constexpr unsigned nbWeeks = 4;
    {
        OptimisationsSimulationTable tables{nullptr};
        LegacySimulationTablesWriter writer(tempDir, 22, TableFormat::Parquet);
        for (unsigned w = 0; w < nbWeeks; ++w)
        {
            addWeekEntries(*tables.firstOptimSimulationTable(), w);
            writer.flush(tables);
        }
        writer.finish(tables);
    }
    const auto path = tablesDir / "simulation-table-22-optim-nb-1.parquet";
    BOOST_REQUIRE(std::filesystem::exists(path));

    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(path.string()));
    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));
    BOOST_CHECK_EQUAL(reader->num_row_groups(), static_cast<int>(nbWeeks));
    std::shared_ptr<arrow::Table> table;
    PARQUET_ASSIGN_OR_THROW(table, reader->ReadTable());
    BOOST_CHECK_EQUAL(table->num_rows(), static_cast<int64_t>(nbWeeks * 3));
    reader.reset();
    PARQUET_THROW_NOT_OK(infile->Close());
    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_CASE(WriteTo_ParquetFormat_CreatesCorrectFiles)
{
    OptimisationsSimulationTable tables{nullptr};
    tables.firstOptimSimulationTable()->addEntry({.block = 1,
                                                  .component = "comp1",
                                                  .output = "var1",
                                                  .absolute_time_index = 1,
                                                  .block_time_index = 1,
                                                  .scenario_index = 0,
                                                  .value = 10.0,
                                                  .status = MipBasisStatus::BASIC});
    tables.secondOptimSimulationTable()->addEntry({.block = 2,
                                                   .component = "comp2",
                                                   .output = "var2",
                                                   .absolute_time_index = 2,
                                                   .block_time_index = 2,
                                                   .scenario_index = 1,
                                                   .value = 20.0,
                                                   .status = MipBasisStatus::FREE});

    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    LegacySimulationTablesWriter(tempDir, 1, TableFormat::Parquet).write(tables);

    const auto file1 = tablesDir / "simulation-table-1-optim-nb-1.parquet";
    const auto file2 = tablesDir / "simulation-table-1-optim-nb-2.parquet";

    // Helper to verify file content
    auto verifyParquet = [](const std::filesystem::path& path)
    {
        BOOST_CHECK(std::filesystem::exists(path));
        std::shared_ptr<arrow::io::ReadableFile> infile;
        PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(path.string()));
        std::unique_ptr<parquet::arrow::FileReader> reader;
        PARQUET_ASSIGN_OR_THROW(reader,
                                parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));
        auto table_or = reader->ReadTable();
        BOOST_CHECK(table_or.ok());
        auto readTable = *table_or;
        BOOST_CHECK_EQUAL(readTable->num_rows(), 1);
        BOOST_CHECK_EQUAL(readTable->num_columns(), 8);
    };

    verifyParquet(file1);
    verifyParquet(file2);

    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_CASE(TableForStage_CreatesOnDemandAndKeepsPointersStable)
{
    OptimisationsSimulationTable tables{nullptr};
    BOOST_CHECK(tables.stages().empty());

    SimulationTable* remix = tables.tableForStage(Stage::peakShaving);
    BOOST_REQUIRE(remix != nullptr);
    BOOST_CHECK_EQUAL(tables.stages().size(), 1u);

    // Asking again for the same stage returns the same table.
    BOOST_CHECK(tables.tableForStage(Stage::peakShaving) == remix);

    // Adding stages must not invalidate pointers already handed out:
    // OPT_OptimisationLineaire grabs the optim-nb-1 table before optim-nb-2 exists.
    SimulationTable* first = tables.firstOptimSimulationTable();
    SimulationTable* second = tables.secondOptimSimulationTable();
    BOOST_CHECK(first != second);
    BOOST_CHECK(tables.tableForStage(Stage::peakShaving) == remix);
    BOOST_CHECK(tables.tableForStage(Stage::firstOptim) == first);
    BOOST_CHECK_EQUAL(tables.stages().size(), 3u);

    remix->addEntry({.block = 0,
                     .component = "comp",
                     .output = "var",
                     .absolute_time_index = 0,
                     .block_time_index = 0,
                     .scenario_index = 0,
                     .value = 1.0,
                     .status = std::nullopt});
    BOOST_CHECK_EQUAL(remix->rowCount(), 1u);

    // clear() empties every table but keeps the stages, which recur every year.
    tables.clear();
    BOOST_CHECK_EQUAL(tables.stages().size(), 3u);
    BOOST_CHECK_EQUAL(remix->rowCount(), 0u);
}

BOOST_AUTO_TEST_CASE(WriteTo_NamesOneFilePerStage)
{
    OptimisationsSimulationTable tables{nullptr};
    const SimulationTableEntry entry{.block = 1,
                                     .component = "comp1",
                                     .output = "var1",
                                     .absolute_time_index = 1,
                                     .block_time_index = 1,
                                     .scenario_index = 0,
                                     .value = 10.0,
                                     .status = MipBasisStatus::BASIC};
    tables.firstOptimSimulationTable()->addEntry(entry);
    tables.secondOptimSimulationTable()->addEntry(entry);
    tables.tableForStage(Stage::peakShaving)->addEntry(entry);

    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    LegacySimulationTablesWriter(tempDir, 7 /* year */, TableFormat::CSV).write(tables);

    for (const auto* name: {"simulation-table-7-optim-nb-1.csv",
                            "simulation-table-7-optim-nb-2.csv",
                            "simulation-table-7-peak-shaving.csv"})
    {
        const auto file = tablesDir / name;
        BOOST_CHECK_MESSAGE(std::filesystem::exists(file), "missing " + std::string(name));
    }
    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_CASE(WriteTo_SkipsStagesWithNoRows)
{
    OptimisationsSimulationTable tables{nullptr};
    tables.firstOptimSimulationTable()->addEntry({.block = 1,
                                                  .component = "comp1",
                                                  .output = "var1",
                                                  .absolute_time_index = 1,
                                                  .block_time_index = 1,
                                                  .scenario_index = 0,
                                                  .value = 10.0,
                                                  .status = MipBasisStatus::BASIC});
    // Asked for, but never filled -- a post-process dump that declined to run.
    tables.tableForStage(Stage::peakShaving);

    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    LegacySimulationTablesWriter(tempDir, 8 /* year */, TableFormat::CSV).write(tables);

    const auto filled = tablesDir / "simulation-table-8-optim-nb-1.csv";
    const auto empty = tablesDir / "simulation-table-8-peak-shaving.csv";
    BOOST_CHECK(std::filesystem::exists(filled));
    BOOST_CHECK_MESSAGE(!std::filesystem::exists(empty),
                        "an empty stage must not produce a header-only file");

    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_AcceptsAllTheStageNames)
{
    for (const auto stage: allStages)
    {
        const auto selection = OptimisationsSimulationTable::parseStageSelection(
          std::string(stageName(stage)));
        BOOST_CHECK_EQUAL(selection.size(), 1u);
        BOOST_CHECK(selection.contains(stage));
    }
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_AllMeansNoRestriction)
{
    BOOST_CHECK(OptimisationsSimulationTable::parseStageSelection("all").empty());
    // "all" wins over its neighbours rather than being taken for a stage name,
    // wherever in the list it sits.
    BOOST_CHECK(OptimisationsSimulationTable::parseStageSelection("peak-shaving,all").empty());
    BOOST_CHECK(OptimisationsSimulationTable::parseStageSelection("all,peak-shaving").empty());

    // Widening the selection is not a licence to stop reading: a name after an
    // "all" is still checked, so a typo is reported rather than swallowed.
    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection("all,optim-nb-3"),
                      Antares::Error::InvalidArgumentError);
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_RejectsAListWithNoUsableName)
{
    // Unlike an absent selection (which the caller never brings here), an empty
    // one reads like a deliberate "no stage" -- which this option cannot mean.
    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection(""),
                      Antares::Error::InvalidArgumentError);
    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection("   "),
                      Antares::Error::InvalidArgumentError);
    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection(" , , "),
                      Antares::Error::InvalidArgumentError);
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_LastStandsForTheGivenStage)
{
    const auto remix = OptimisationsSimulationTable::parseStageSelection(
      "last",
      "--simulation-table-stages",
      Stage::peakShaving);
    BOOST_CHECK_EQUAL(remix.size(), 1u);
    BOOST_CHECK(remix.contains(Stage::peakShaving));

    const auto csr = OptimisationsSimulationTable::parseStageSelection("last",
                                                                       "--simulation-table-stages",
                                                                       Stage::adequacyPatch);
    BOOST_CHECK_EQUAL(csr.size(), 1u);
    BOOST_CHECK(csr.contains(Stage::adequacyPatch));

    // "last" combines with explicit stage names like any other token.
    const auto mixed = OptimisationsSimulationTable::parseStageSelection(
      "optim-nb-1, last",
      "--simulation-table-stages",
      Stage::peakShaving);
    BOOST_CHECK_EQUAL(mixed.size(), 2u);
    BOOST_CHECK(mixed.contains(Stage::firstOptim));
    BOOST_CHECK(mixed.contains(Stage::peakShaving));
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_TrimsSpacesAndRejectsUnknownNames)
{
    const auto selection = OptimisationsSimulationTable::parseStageSelection(
      " optim-nb-2 , adq-patch ");
    BOOST_CHECK_EQUAL(selection.size(), 2u);
    BOOST_CHECK(selection.contains(Stage::secondOptim));
    BOOST_CHECK(selection.contains(Stage::adequacyPatch));

    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection("optim-nb-3"),
                      Antares::Error::InvalidArgumentError);
    // A stage name that is only a prefix of a real one is still a mistake.
    BOOST_CHECK_THROW((void)OptimisationsSimulationTable::parseStageSelection("peak"),
                      Antares::Error::InvalidArgumentError);
}

BOOST_AUTO_TEST_CASE(ParseStageSelection_ErrorNamesWhereTheListCameFrom)
{
    // The same list can come from the command line or from generaldata.ini;
    // pointing at the wrong one sends the reader to the wrong file.
    try
    {
        (void)OptimisationsSimulationTable::parseStageSelection("nope", "some-source");
        BOOST_FAIL("an unknown stage name must throw");
    }
    catch (const Antares::Error::InvalidArgumentError& e)
    {
        const std::string message = e.what();
        BOOST_CHECK_MESSAGE(message.find("some-source") != std::string::npos, message);
        BOOST_CHECK_MESSAGE(message.find("peak-shaving") != std::string::npos, message);
    }
}

BOOST_AUTO_TEST_CASE(SelectStages_UnselectedStagesReturnNullptr)
{
    OptimisationsSimulationTable tables{nullptr};
    tables.selectStages({Stage::peakShaving});

    BOOST_CHECK(tables.tableForStage(Stage::peakShaving) != nullptr);
    BOOST_CHECK(tables.firstOptimSimulationTable() == nullptr);
    BOOST_CHECK(tables.secondOptimSimulationTable() == nullptr);
    BOOST_CHECK(tables.tableForStage(Stage::adequacyPatch) == nullptr);

    // A refused stage is not even created, so the writer never sees it.
    BOOST_CHECK_EQUAL(tables.stages().size(), 1u);
}

BOOST_AUTO_TEST_CASE(SelectStages_PostProcessStagesDriveTheModelerProblemRetention)
{
    // The weekly solve keeps its modeler problem alive only for a post-process
    // dump to re-emit; asking the question must not depend on -- nor create --
    // an optimisation-pass table, since a selection may well have none.
    OptimisationsSimulationTable everything{nullptr};
    BOOST_CHECK(everything.anyPostProcessStageSelected());

    for (const auto stage: {Stage::peakShaving, Stage::adequacyPatch})
    {
        OptimisationsSimulationTable tables{nullptr};
        tables.selectStages({stage});
        BOOST_CHECK_MESSAGE(tables.anyPostProcessStageSelected(), stageName(stage));
        // Specifically the case that used to drop the modeler rows: no optim
        // stage is selected, so neither pass gets a table of its own.
        BOOST_CHECK(tables.firstOptimSimulationTable() == nullptr);
        BOOST_CHECK(tables.secondOptimSimulationTable() == nullptr);
    }

    OptimisationsSimulationTable optimOnly{nullptr};
    optimOnly.selectStages({Stage::firstOptim, Stage::secondOptim});
    BOOST_CHECK(!optimOnly.anyPostProcessStageSelected());

    // Querying must not create anything.
    OptimisationsSimulationTable untouched{nullptr};
    untouched.selectStages({Stage::peakShaving});
    BOOST_CHECK(untouched.isStageSelected(Stage::peakShaving));
    BOOST_CHECK(!untouched.isStageSelected(Stage::firstOptim));
    BOOST_CHECK(untouched.anyPostProcessStageSelected());
    BOOST_CHECK(untouched.stages().empty());
}

BOOST_AUTO_TEST_CASE(SelectStages_EmptySelectionKeepsEveryStage)
{
    OptimisationsSimulationTable tables{nullptr};
    tables.selectStages({});

    for (const auto stage: allStages)
    {
        BOOST_CHECK_MESSAGE(tables.tableForStage(stage) != nullptr,
                            "refused " + std::string(stageName(stage)));
    }
}

BOOST_AUTO_TEST_CASE(SelectStages_WriterOnlyEmitsSelectedStages)
{
    OptimisationsSimulationTable tables{nullptr};
    tables.selectStages({Stage::firstOptim});

    const SimulationTableEntry entry{.block = 1,
                                     .component = "comp1",
                                     .output = "var1",
                                     .absolute_time_index = 1,
                                     .block_time_index = 1,
                                     .scenario_index = 0,
                                     .value = 10.0,
                                     .status = MipBasisStatus::BASIC};
    tables.firstOptimSimulationTable()->addEntry(entry);

    auto tempDir = makeTemporaryOutputDirectory();
    const auto tablesDir = simulationTableOutputDirectory(tempDir);
    LegacySimulationTablesWriter(tempDir, 9 /* year */, TableFormat::CSV).write(tables);

    const auto selected = tablesDir / "simulation-table-9-optim-nb-1.csv";
    const auto refused = tablesDir / "simulation-table-9-optim-nb-2.csv";
    BOOST_CHECK(std::filesystem::exists(selected));
    BOOST_CHECK_MESSAGE(!std::filesystem::exists(refused),
                        "an unselected stage must not produce a file");

    std::filesystem::remove_all(tempDir);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(VariableDictionaryTests)

BOOST_AUTO_TEST_CASE(BuildVariableName_AllParameters)
{
    std::string result = buildVariableName("component1",
                                           "variable1",
                                           MCYearAndTime::MCYear{5u},
                                           10u);
    BOOST_CHECK_EQUAL(result, "component1.variable1_s5_t10");
}

BOOST_AUTO_TEST_CASE(BuildVariableName_OnlyScenario)
{
    std::string result = buildVariableName("component1",
                                           "variable1",
                                           MCYearAndTime::MCYear{5u},
                                           std::nullopt);
    BOOST_CHECK_EQUAL(result, "component1.variable1_s5");
}

BOOST_AUTO_TEST_CASE(BuildVariableName_OnlyTimestep)
{
    std::string result = buildVariableName("component1", "variable1", std::nullopt, 10u);
    BOOST_CHECK_EQUAL(result, "component1.variable1_t10");
}

BOOST_AUTO_TEST_CASE(BuildVariableName_NoOptionalParams)
{
    std::string result = buildVariableName("component1", "variable1", std::nullopt, std::nullopt);
    BOOST_CHECK_EQUAL(result, "component1.variable1");
}

BOOST_AUTO_TEST_SUITE_END()

// Mock solver traits for testing
struct MockSolverTraits
{
    static double getValue(const MockMipVariable* var)
    {
        return var ? var->solutionValue() : 0.0;
    }

    static MipBasisStatus getStatus(const MockMipVariable* var)
    {
        return var ? var->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE;
    }

    static MipBasisStatus getStatus(const MockMipConstraint* cst)
    {
        return cst ? cst->getMipBasisStatus() : MipBasisStatus::NOT_AVAILABLE;
    }

    static std::optional<double> getValue(const MockMipConstraint*)
    {
        return std::nullopt;
    }
};

struct MockMipSolution: IMipSolution
{
    [[nodiscard]] MipStatus getStatus() const override
    {
        return MipStatus::OPTIMAL;
    }

    [[nodiscard]] double getObjectiveValue() const override
    {
        return 11.18;
    }
};

struct BasicProblemFixture: Test::Modeler::LinearProblemBuildingFixture, SimulationTableFileFixture
{
    void addRandomVariables(const FillContext& fillContext,
                            MockLinearProblem* linearProblem,
                            const Component& compo) const
    {
        for (const auto& var: compo.getModel()->Variables())
        {
            optimEntityContainer->addStartColumn();
            if (var.isTimeDependent())
            {
                for (unsigned t = 0; t < fillContext.getLocalNumberOfTimeSteps(); ++t)
                {
                    linearProblem->addVariable(0, 0, false, "");
                }
            }
            else
            {
                linearProblem->addVariable(0, 0, false, "");
            }
        }
    }

    void addRandomConstraints(const FillContext& fillContext,
                              MockLinearProblem* linearProblem,
                              const Component& compo)
    {
        auto* model = compo.getModel();

        for (const auto& constraint: model->Constraints())
        {
            const auto& constraintId = constraint.Id();
            const auto ctVariability = VariabilityVisitor(*optimEntityContainer,
                                                          compo,
                                                          linearProblem->isLP())
                                         .dispatch(constraint.expression().RootNode());
            unsigned ctCount = 1;
            if (isTimeDependent(ctVariability))
            {
                ctCount = fillContext.getLocalNumberOfTimeSteps();
                for (unsigned t = 0; t < fillContext.getLocalNumberOfTimeSteps(); ++t)
                {
                    linearProblem->addConstraint(0, 0, "");
                }
            }
            else
            {
                linearProblem->addConstraint(0, 0, "");
            }
            optimEntityContainer->registerConstraint(compo, ctVariability, ctCount);
        }
    }

    void setOptimEntityContainer(MockLinearProblem* linearProblem)
    {
        optimEntityContainer = std::make_unique<OptimEntityContainer>(*linearProblem);
    }

    void PrepareData()
    {
        std::vector<Test::Modeler::VariableData> variablesData = {
          {"var1", ValueType::FLOAT, nullptr, nullptr, false, false}, // Neither time nor scenario
                                                                      // dependent
          {"var2", ValueType::FLOAT, nullptr, nullptr, true, false},  // Scenario dependent only
          {"var3", ValueType::FLOAT, nullptr, nullptr, false, true},  // Time dependent only
          {"var4", ValueType::FLOAT, nullptr, nullptr, true, true}    // Both time and scenario
                                                                      // dependent
        };
        auto var1_node = variable("var1", 0);
        auto var4_node = variable("var4", 8954, VariabilityType::VARYING_IN_TIME_AND_SCENARIO);
        auto three = literal(3);
        auto ct1_node = nodeRegistry.create<Nodes::LessThanOrEqualNode>(var1_node, three);
        auto ct2_node = nodeRegistry.create<Nodes::GreaterThanOrEqualNode>(multiply(var1_node,
                                                                                    var4_node),
                                                                           three);
        std::vector<Test::Modeler::ConstraintData> constraintsData = {{"constraint1", ct1_node},
                                                                      {"constraint2", ct2_node}};
        createModel("model", {}, variablesData, constraintsData, nullptr);
        createComponent("model", "comp1", {}, "Shutter Island");
        createComponent("model", "comp2", {}, "Inside Man");
    }

    void AddRandomVariablesAndContraints(const FillContext& fillContext,
                                         MockLinearProblem* linearProblem)
    {
        for (const auto& compo: components)
        {
            const auto& compoId = compo.Id();
            scenarioGroupRepo.addScenario(compo.getScenarioGroupId(),
                                          std::make_unique<Scenario>(compo.getScenarioGroupId()));
            addRandomVariables(fillContext, linearProblem, compo);
        }

        optimEntityContainer->addFromSystemComponents(components);

        for (const auto& compo: components)
        {
            addRandomConstraints(fillContext, linearProblem, compo);
        }
    }

    void build(const FillContext& fillContext = {0, 4, 0, 4, 0},
               MockLinearProblem* linearProblem = nullptr)
    {
        if (!linearProblem)
        {
            return;
        }

        PrepareData();
        setOptimEntityContainer(linearProblem);
        AddRandomVariablesAndContraints(fillContext, linearProblem);
    }

    std::unique_ptr<OptimEntityContainer> optimEntityContainer = nullptr;
};

BOOST_FIXTURE_TEST_SUITE(ComponentModelIntegrationTests, BasicProblemFixture)

auto count_lines = [](std::string_view s)
{
    return rng::count_if(s | std::views::split('\n'),
                         [](auto&& line) { return !rng::empty(line); });
};

BOOST_AUTO_TEST_CASE(TemplateFunction_VariableEntries_AllCombinations)
{
    SimulationTable table;
    MockLinearProblem linearProblem(true);
    const FillContext fillContext(0, 9, 0, 9, 0); // 10 time steps
    build(fillContext, &linearProblem);

    const auto& component = components.front();

    addVariableEntries(table,
                       linearProblem,
                       fillContext,
                       component,
                       *optimEntityContainer,
                       1,
                       TimeConversionMode::SingleBlock,
                       0);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    // Should have entries for all 4 variable types with different time/scenario combinations
    // var1: 1 entry (neither time nor scenario dependent)
    // var2: 1 entry (scenario dependent only)
    // var3: 10 entries (time dependent only, 10 time steps)
    // var4: 10 entries (both dependent, 10 time steps)
    // Total: 22 entries + 1 header = 23 lines
    BOOST_CHECK_EQUAL(count_lines(content), 23);
}

BOOST_AUTO_TEST_SUITE_END()

namespace Antares::LinearProblem::Api
{

inline std::ostream& operator<<(std::ostream& os, const MipBasisStatus& status)
{
    return os << StatusToString(status);
}

} // namespace Antares::LinearProblem::Api

BOOST_FIXTURE_TEST_SUITE(SimulationTableGeneratorTemplateTests, BasicProblemFixture)

BOOST_AUTO_TEST_CASE(FillSimulationTable_ModelerIntegration)
{
    SimulationTable table;
    FillContext fillContext(0, 4, 0, 4, 0); // 5 time steps
    MockLinearProblem linearProblem(true);
    LinearProblemData data;

    build(fillContext, &linearProblem);
    MockMipSolution solution;
    BOOST_CHECK_NO_THROW(FillSimulationTable(table,
                                             linearProblem,
                                             45.0,
                                             getModelerData(),
                                             *optimEntityContainer,
                                             fillContext,
                                             1,
                                             TimeConversionMode::SingleBlock););
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_WeeklyBlockTimeIndexUsesLocalStep)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 168, 169, 0); // 1 local time steps, week 2 globally
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        1,
                        TimeConversionMode::WeeklyBlocks);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,var4,168,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var4,169,1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_DailyBlockTimeIndexUsesLocalStep)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 24, 25, 0); // 1 local time steps, day 2 globally
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        1,
                        TimeConversionMode::DailyBlocks);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,var4,24,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var4,25,1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_SingleBlockTimeIndexUsesLocalStep)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 24, 25, 0); // 2 local time steps, single block
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        0,
                        TimeConversionMode::SingleBlock);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,comp1,var4,0,0") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var4,1,1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_WeeklyBlockConstraintTimeIndexUsesLocalStep)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 168, 169, 0); // 2 local time steps, week 2 globally
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        1,
                        TimeConversionMode::WeeklyBlocks);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,constraint2,168,0,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,constraint2,169,1,0") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_ForceScenarioIndexForTimeOnlyVariables)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 0, 1, 0); // 2 local time steps
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        0,
                        TimeConversionMode::SingleBlock,
                        true);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,comp1,constraint1,None,None,0") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_BlockTimeIndexAbsentForScenarioOnlyOutputs)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 0, 1, 0); // 2 local time steps
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        0,
                        TimeConversionMode::SingleBlock);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,comp1,var3,None,None,0") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_VariabilityCombinations)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 0, 1, 0); // 2 local time steps
    MockLinearProblem linearProblem(true);

    build(fillContext, &linearProblem);
    FillSimulationTable(table,
                        linearProblem,
                        45.0,
                        getModelerData(),
                        *optimEntityContainer,
                        fillContext,
                        0,
                        TimeConversionMode::SingleBlock);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,comp1,var1,None,None,0,") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var2,0,0,0") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var2,1,1,0") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var3,None,None,0") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var4,0,0,0") != std::string::npos);
    BOOST_CHECK(content.find("0,comp1,var4,1,1,0") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_SkipsDroppedDualExtraOutputTimesteps)
{
    auto* currentVarNode = variable("var1", 0, VariabilityType::VARYING_IN_TIME_ONLY);
    auto* nextVarNode = nodeRegistry.create<Nodes::TimeShiftNode>(currentVarNode, literal(1));

    createModel("model",
                {},
                {{"var1", ValueType::BOOL, literal(0), literal(1), true, false}},
                {{"ct_drop",
                  nodeRegistry.create<Nodes::EqualNode>(nextVarNode, currentVarNode),
                  OutOfBoundsProcessingMode::DROP},
                 {"ct_cyclic",
                  nodeRegistry.create<Nodes::EqualNode>(nextVarNode, currentVarNode),
                  OutOfBoundsProcessingMode::CYCLIC}});
    createComponent("model", "componentToto");

    FillContext fillContext(0, 2, 0, 2, 0);
    pb = std::make_unique<OrtoolsLinearProblem>(true, "scip");
    optimEntityContainer = std::make_unique<OptimEntityContainer>(*pb);
    optimEntityContainer->addFromSystemComponents(components);

    std::vector<std::unique_ptr<LinearProblemFiller>> fillers;
    for (auto& component: components)
    {
        fillers.push_back(std::make_unique<ComponentFiller>(component,
                                                            &dummy_data_,
                                                            *optimEntityContainer,
                                                            scenarioGroupRepo,
                                                            Config::Location::SUBPROBLEMS));
    }
    LinearProblemBuilder(fillers).build(fillContext);
    BOOST_REQUIRE(pb->solve(false));

    auto& modelerData = getModelerData();
    SimulationTable table;
    FillSimulationTable(table,
                        *pb,
                        45.0,
                        modelerData,
                        *optimEntityContainer,
                        fillContext,
                        0,
                        TimeConversionMode::SingleBlock);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find(",componentToto,ct_drop,0,0,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_drop,1,1,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_drop,2,2,") == std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,0,0,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,1,1,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,2,2,") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EdgeCaseStressTests)

BOOST_FIXTURE_TEST_CASE(EmptyStrings_AllFields, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 0,
                               .component = "",
                               .output = "",
                               .absolute_time_index = std::nullopt,
                               .block_time_index = std::nullopt,
                               .scenario_index = 0,
                               .value = std::nullopt,
                               .status = std::nullopt};

    table.addEntry(entry);

    simulation_table_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,,,None,None,0,None,None") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TimeConversionTests)

BOOST_AUTO_TEST_CASE(ConvertTimeStep)
{
    auto result = convertBlockTimeStepToAbsoluteTimeStep(42, TimeConversionMode::SingleBlock, 45);
    BOOST_CHECK_EQUAL(result.block, 0);
    BOOST_CHECK_EQUAL(*result.blockTimeIndex, 42);
    BOOST_CHECK_EQUAL(*result.absoluteTimeIndex, 42);
    auto result0 = convertBlockTimeStepToAbsoluteTimeStep(0, TimeConversionMode::SingleBlock, 99);
    BOOST_CHECK_EQUAL(result0.block, 0);
    BOOST_CHECK_EQUAL(*result0.blockTimeIndex, 0);
    BOOST_CHECK_EQUAL(*result0.absoluteTimeIndex, 0);

    // Daily blocks - exactly at day boundary
    auto result1 = convertBlockTimeStepToAbsoluteTimeStep(23, TimeConversionMode::DailyBlocks, 0);
    BOOST_CHECK_EQUAL(result1.block, 0);
    BOOST_CHECK_EQUAL(*result1.blockTimeIndex, 23);
    BOOST_CHECK_EQUAL(*result1.absoluteTimeIndex, 23);

    auto result2 = convertBlockTimeStepToAbsoluteTimeStep(10, TimeConversionMode::DailyBlocks, 2);
    BOOST_CHECK_EQUAL(result2.block, 2);
    BOOST_CHECK_EQUAL(*result2.blockTimeIndex, 10);
    BOOST_CHECK_EQUAL(*result2.absoluteTimeIndex, 58);

    // Weekly blocks - exactly at week boundary
    auto result3 = convertBlockTimeStepToAbsoluteTimeStep(10, TimeConversionMode::WeeklyBlocks, 0);
    BOOST_CHECK_EQUAL(result3.block, 0);
    BOOST_CHECK_EQUAL(*result3.blockTimeIndex, 10);
    BOOST_CHECK_EQUAL(*result3.absoluteTimeIndex, 10);

    auto result4 = convertBlockTimeStepToAbsoluteTimeStep(0, TimeConversionMode::WeeklyBlocks, 1);
    BOOST_CHECK_EQUAL(result4.block, 1);
    BOOST_CHECK_EQUAL(*result4.blockTimeIndex, 0);
    BOOST_CHECK_EQUAL(*result4.absoluteTimeIndex, 168);

    auto result5 = convertBlockTimeStepToAbsoluteTimeStep(167, TimeConversionMode::WeeklyBlocks, 1);
    BOOST_CHECK_EQUAL(result5.block, 1);
    BOOST_CHECK_EQUAL(*result5.blockTimeIndex, 167);
    BOOST_CHECK_EQUAL(*result5.absoluteTimeIndex, 335);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ConvertOrtoolsBasisStatusTests)

BOOST_AUTO_TEST_CASE(ConvertAllOrtoolsStatuses)
{
    using OrtoolsStatus = operations_research::MPSolver::BasisStatus;

    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(OrtoolsStatus::FREE), MipBasisStatus::FREE);
    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(OrtoolsStatus::AT_LOWER_BOUND),
                      MipBasisStatus::AT_LOWER_BOUND);
    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(OrtoolsStatus::AT_UPPER_BOUND),
                      MipBasisStatus::AT_UPPER_BOUND);
    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(OrtoolsStatus::FIXED_VALUE),
                      MipBasisStatus::FIXED_VALUE);
    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(OrtoolsStatus::BASIC), MipBasisStatus::BASIC);
}

BOOST_AUTO_TEST_CASE(ConvertInvalidStatus_ReturnsNotAvailable)
{
    // Test with an invalid status value
    auto invalidStatus = static_cast<operations_research::MPSolver::BasisStatus>(999);
    BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(invalidStatus), MipBasisStatus::NOT_AVAILABLE);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SimulationTableGeneratorTests)

BOOST_AUTO_TEST_CASE(BuildModelerConstraintName_AllParameters)
{
    std::string result = BuildModelerConstraintName("comp1", "constraint1", 10u);
    BOOST_CHECK_EQUAL(result, "comp1.constraint1_10");
}

BOOST_AUTO_TEST_CASE(BuildModelerConstraintName_NoOptionalParams)
{
    std::string result = BuildModelerConstraintName("comp1", "constraint1", std::nullopt);
    BOOST_CHECK_EQUAL(result, "comp1.constraint1");
}

BOOST_AUTO_TEST_SUITE_END()
