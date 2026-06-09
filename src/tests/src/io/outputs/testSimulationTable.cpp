// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <stdexcept>
#define WIN32_LEAN_AND_MEAN

#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

// Mock includes for testing - replace with actual includes
#include <inmemory-modeler.h>
#include <unit_test_utils.h>

#include "antares/expressions/visitors/VariabilityVisitor.h"
#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTableEntry.h"
#include "antares/io/outputs/SimulationTableGenerator.h"
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

#include "../private/csv_table_writer.h"
#include "UtilMocks.h"

using namespace Antares::Optimisation::LinearProblemApi;
using namespace Antares::Optimisation::LinearProblemMpsolverImpl;

using namespace std;
using namespace Antares::Optimization;
using namespace Antares::Optimisation;
using namespace Antares::Optimisation::LinearProblemDataImpl;
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
    using TI = Antares::Optimisation::VariabilityType;
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
        csv_writer(out_file_path)
    {
        remove_if_exists();
    }

    ~SimulationTableFileFixture()
    {
        remove_if_exists();
    }

    fs::path out_file_path;
    CsvTableWriter csv_writer;

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
    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

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
    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

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

BOOST_AUTO_TEST_CASE(MultipleEntries)
{
    SimulationTable table;
    const size_t numEntries = 3;
    for (size_t i = 0; i < numEntries; ++i)
    {
        SimulationTableEntry entry{.block = static_cast<unsigned>(i + 1),
                                   .component = "comp" + std::to_string(i),
                                   .output = "var" + std::to_string(i),
                                   .absolute_time_index = i * 10,
                                   .block_time_index = i % 168,
                                   .scenario_index = static_cast<unsigned>(i % 10),
                                   .value = static_cast<double>(i) * 0.1,
                                   .status = static_cast<MipBasisStatus>(i % 6)};
        table.addEntry(entry);
    }

    // Count lines (should be numEntries)
    BOOST_CHECK_EQUAL(table.rowCount(), numEntries);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FileWriterIntegrationTests)

BOOST_AUTO_TEST_CASE(WriteTo_CreatesCorrectFiles)
{
    OptimisationsSimulationTable tables;

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

    auto tempDir = std::filesystem::temp_directory_path();
    LegacySimulationTablesWriter legacyWriter(tempDir, 1 /* year */);
    legacyWriter.write(tables);

    // Check that both CSV files were created
    auto file1 = tempDir / "simulation-table-1-optim-nb-1.csv";
    auto file2 = tempDir / "simulation-table-1-optim-nb-2.csv";

    BOOST_CHECK(std::filesystem::exists(file1));
    BOOST_CHECK(std::filesystem::exists(file2));

    // Read and verify content of first file
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
    std::filesystem::remove(file1);
    std::filesystem::remove(file2);
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

BOOST_AUTO_TEST_SUITE(ThreadSafetyTests)

BOOST_AUTO_TEST_CASE(ConcurrentAccess_MultipleThreads)
{
    SimulationTable table;
    const int numThreads = 4;
    const int entriesPerThread = 100;

    std::vector<std::thread> threads;
    std::mutex tableMutex; // Protect table access in test

    for (int t = 0; t < numThreads; ++t)
    {
        threads.emplace_back(
          [&table, &tableMutex, t, entriesPerThread]()
          {
              for (int i = 0; i < entriesPerThread; ++i)
              {
                  SimulationTableEntry entry{.block = static_cast<unsigned>(t * entriesPerThread
                                                                            + i),
                                             .component = "comp_" + std::to_string(t),
                                             .output = "var_" + std::to_string(i),
                                             .absolute_time_index = i,
                                             .block_time_index = i % 24,
                                             .scenario_index = static_cast<unsigned>(t),
                                             .value = static_cast<double>(i),
                                             .status = MipBasisStatus::BASIC};

                  std::lock_guard<std::mutex> lock(tableMutex);
                  table.addEntry(entry);
              }
          });
    }

    for (auto& thread: threads)
    {
        thread.join();
    }

    // Should have all entries
    BOOST_CHECK_EQUAL(table.rowCount(), numThreads * entriesPerThread);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PerformanceTests)

BOOST_AUTO_TEST_CASE(WritePerformance_LargeDataSet)
{
    SimulationTable table;
    const int numEntries = 50000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numEntries; ++i)
    {
        SimulationTableEntry entry{.block = static_cast<unsigned>(i % 1000 + 1),
                                   .component = "component_" + std::to_string(i % 100),
                                   .output = "variable_" + std::to_string(i % 50),
                                   .absolute_time_index = i,
                                   .block_time_index = i % 168,
                                   .scenario_index = static_cast<unsigned>(i % 10),
                                   .value = static_cast<double>(i) * 0.001,
                                   .status = static_cast<MipBasisStatus>(i % 6)};
        table.addEntry(entry);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Performance check - should complete within reasonable time
    BOOST_CHECK_LT(duration.count(), 5000); // Less than 5 seconds

    // Verify all data was written
    BOOST_CHECK_EQUAL(table.rowCount(), numEntries);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BoundaryValueTests)

BOOST_AUTO_TEST_CASE(DoubleValues_PrecisionBoundaries)
{
    SimulationTable table;

    // Test various double precision scenarios
    std::vector<double> testValues = {0.0,
                                      -0.0,
                                      std::numeric_limits<double>::min(),
                                      std::numeric_limits<double>::max(),
                                      std::numeric_limits<double>::epsilon(),
                                      1.0 / 3.0, // Repeating decimal
                                      1e-10,     // Very small
                                      1e10,      // Very large
                                      std::numeric_limits<double>::infinity(),
                                      -std::numeric_limits<double>::infinity()};

    for (size_t i = 0; i < testValues.size(); ++i)
    {
        SimulationTableEntry entry{.block = static_cast<unsigned>(i + 1),
                                   .component = "comp",
                                   .output = "var",
                                   .absolute_time_index = static_cast<unsigned>(i),
                                   .block_time_index = static_cast<unsigned>(i),
                                   .scenario_index = 0,
                                   .value = testValues[i],
                                   .status = MipBasisStatus::BASIC};

        BOOST_CHECK_NO_THROW(table.addEntry(entry));
    }

    BOOST_CHECK(!table.columns().empty());
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
            const auto constraint_variability = VariabilityVisitor(*optimEntityContainer, compo)
                                                  .dispatch(constraint.expression().RootNode());
            optimEntityContainer->registerConstraint(compo, constraint_variability);
            if (isTimeDependent(constraint_variability))
            {
                for (unsigned t = 0; t < fillContext.getLocalNumberOfTimeSteps(); ++t)
                {
                    linearProblem->addConstraint(0, 0, "");
                }
            }
            else
            {
                linearProblem->addConstraint(0, 0, "");
            }
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

    csv_writer.writeTable(table);
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

BOOST_AUTO_TEST_SUITE(DataIntegrityTests)

BOOST_FIXTURE_TEST_CASE(RoundTrip_DataIntegrity, SimulationTableFileFixture)
{
    // Test that data written matches data read
    SimulationTable table;

    std::vector<SimulationTableEntry> originalEntries = {
      {1, "comp1", "var1", 10, 5, 1, 123.456, MipBasisStatus::BASIC},
      {2, "comp2", "var2", std::nullopt, std::nullopt, 0, std::nullopt, std::nullopt},
      {3, "comp3", "var3", 20, 10, 2, -456.789, MipBasisStatus::AT_UPPER_BOUND}};

    for (const auto& entry: originalEntries)
    {
        table.addEntry(entry);
    }

    csv_writer.writeTable(table);

    // Parse the CSV output manually to verify data integrity
    std::ifstream file_istream(out_file_path);
    std::string line;

    // Check that the column names are there.
    std::getline(file_istream, line);
    std::string names = "block,component,output,absolute_time_index,block_time_index,"
                        "scenario_index,value,basis_status";
    BOOST_CHECK(line.find(names) != std::string::npos);

    size_t entryIndex = 0;
    while (std::getline(file_istream, line) && entryIndex < originalEntries.size())
    {
        const auto& original = originalEntries[entryIndex];

        // Basic checks that line contains expected components
        BOOST_CHECK(line.find(std::to_string(original.block)) != std::string::npos);
        BOOST_CHECK(line.find(original.component.value()) != std::string::npos);
        BOOST_CHECK(line.find(original.output) != std::string::npos);

        entryIndex++;
    }

    BOOST_CHECK_EQUAL(entryIndex, originalEntries.size());
}

BOOST_AUTO_TEST_SUITE_END()

namespace Antares::Optimisation::LinearProblemApi
{

inline std::ostream& operator<<(std::ostream& os, const MipBasisStatus& status)
{
    return os << StatusToString(status);
}

} // namespace Antares::Optimisation::LinearProblemApi

BOOST_AUTO_TEST_SUITE(StatusConversionComprehensiveTests)

BOOST_AUTO_TEST_CASE(OrtoolsToMipStatus_AllMappings)
{
    using OrtoolsStatus = operations_research::MPSolver::BasisStatus;

    // Test all valid mappings
    std::vector<std::pair<OrtoolsStatus, MipBasisStatus>> mappings = {
      {OrtoolsStatus::FREE, MipBasisStatus::FREE},
      {OrtoolsStatus::AT_LOWER_BOUND, MipBasisStatus::AT_LOWER_BOUND},
      {OrtoolsStatus::AT_UPPER_BOUND, MipBasisStatus::AT_UPPER_BOUND},
      {OrtoolsStatus::FIXED_VALUE, MipBasisStatus::FIXED_VALUE},
      {OrtoolsStatus::BASIC, MipBasisStatus::BASIC}};

    for (const auto& [ortoolsStatus, expectedMipStatus]: mappings)
    {
        BOOST_CHECK_EQUAL(convertOrtoolsBasisStatus(ortoolsStatus), expectedMipStatus);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SpecialCharacterTests)

BOOST_FIXTURE_TEST_CASE(UnicodeCharacters_InNames, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "cömpönént_测试",
                               .output = "vär_αβγ",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 42.0,
                               .status = MipBasisStatus::BASIC};

    BOOST_CHECK_NO_THROW(table.addEntry(entry));
    BOOST_CHECK_NO_THROW(csv_writer.writeTable(table));
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("cömpönént_测试") != std::string::npos);
    BOOST_CHECK(content.find("vär_αβγ") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(CSVEscaping_SpecialCharacters, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp,with,commas",
                               .output = "var\"with\"quotes",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 42.0,
                               .status = MipBasisStatus::BASIC};

    table.addEntry(entry);
    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    // Note: This implementation doesn't escape CSV properly, but we show what it actually does
    BOOST_CHECK(content.find("comp,with,commas") != std::string::npos);
    BOOST_CHECK(content.find("var\"\"with\"\"quotes") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("2,comp1,var4,169,1") != std::string::npos);
    BOOST_CHECK(content.find("2,comp1,var4,170,2") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(FillSimulationTable_DailyBlockTimeIndexUsesLocalStep)
{
    SimulationTable table;
    FillContext fillContext(0, 1, 24, 25, 0); // 2 local time steps, day 2 globally
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("2,comp1,var4,25,1") != std::string::npos);
    BOOST_CHECK(content.find("2,comp1,var4,26,2") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,var4,1,1") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var4,2,2") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("2,comp1,constraint2,169,1,0") != std::string::npos);
    BOOST_CHECK(content.find("2,comp1,constraint2,170,2,0") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,constraint1,None,None,0") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,var3,None,None,0") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("1,comp1,var1,None,None,0,") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var2,1,1,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var2,2,2,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var3,None,None,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var4,1,1,0") != std::string::npos);
    BOOST_CHECK(content.find("1,comp1,var4,2,2,0") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find(",componentToto,ct_drop,1,1,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_drop,2,2,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_drop,3,3,") == std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,1,1,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,2,2,") != std::string::npos);
    BOOST_CHECK(content.find(",componentToto,ct_cyclic,3,3,") != std::string::npos);
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

    csv_writer.writeTable(table);
    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("0,,,None,None,0,None,None") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(VeryLongStrings_ComponentNames, SimulationTableFileFixture)
{
    SimulationTable table;
    std::string longComponent(10000, 'a'); // 10k character string
    std::string longOutput(5000, 'b');     // 5k character string

    SimulationTableEntry entry{.block = 1,
                               .component = longComponent,
                               .output = longOutput,
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 1.0,
                               .status = MipBasisStatus::BASIC};

    BOOST_CHECK_NO_THROW(table.addEntry(entry));
    BOOST_CHECK_NO_THROW(csv_writer.writeTable(table));

    std::string content = readFileContent(out_file_path);
    BOOST_CHECK(content.find(longComponent) != std::string::npos);
    BOOST_CHECK(content.find(longOutput) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(AlternatingClear_Write_Operations)
{
    SimulationTable table;

    for (int cycle = 0; cycle < 5; ++cycle)
    {
        // Add entries
        for (int i = 0; i < 3; ++i)
        {
            SimulationTableEntry entry{.block = static_cast<unsigned>(cycle * 3 + i + 1),
                                       .component = "comp_" + std::to_string(i),
                                       .output = "var_" + std::to_string(i),
                                       .absolute_time_index = i,
                                       .block_time_index = i,
                                       .scenario_index = static_cast<unsigned>(cycle),
                                       .value = static_cast<double>(i),
                                       .status = MipBasisStatus::BASIC};
            table.addEntry(entry);
        }

        // Verify content before clearing
        BOOST_CHECK_EQUAL(table.rowCount(), 3);

        // Clear for next cycle
        table.clear();

        // Verify clear worked
        BOOST_CHECK(table.rowCount() == 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SimulationTableCsvFileTests)

BOOST_FIXTURE_TEST_CASE(Write_CreatesFile, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "test_comp",
                               .output = "test_var",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 123.45,
                               .status = MipBasisStatus::BASIC};
    table.addEntry(entry);
    csv_writer.writeTable(table);

    BOOST_CHECK(std::filesystem::exists(out_file_path));

    std::string content = readFileContent(out_file_path);

    BOOST_CHECK(content.find("block,component,output") != std::string::npos);
    BOOST_CHECK(content.find("1,test_comp,test_var,1,1,0,123.45,Basic") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TimeConversionTests)

BOOST_AUTO_TEST_CASE(ConvertTimeStep)
{
    auto result = convertBlockTimeStepToAbsoluteTimeStep(42, TimeConversionMode::SingleBlock, 45);
    BOOST_CHECK_EQUAL(result.block, 1);
    BOOST_CHECK_EQUAL(*result.blockTimeIndex, 43);    // 42 + 1
    BOOST_CHECK_EQUAL(*result.absoluteTimeIndex, 43); // 42 + 1
    auto result0 = convertBlockTimeStepToAbsoluteTimeStep(0, TimeConversionMode::SingleBlock, 99);
    BOOST_CHECK_EQUAL(result0.block, 1);
    BOOST_CHECK_EQUAL(*result0.blockTimeIndex, 1);
    BOOST_CHECK_EQUAL(*result0.absoluteTimeIndex, 1);

    // Daily blocks - exactly at day boundary
    auto result1 = convertBlockTimeStepToAbsoluteTimeStep(23, TimeConversionMode::DailyBlocks, 0);
    BOOST_CHECK_EQUAL(result1.block, 1);
    BOOST_CHECK_EQUAL(*result1.blockTimeIndex, 24);
    BOOST_CHECK_EQUAL(*result1.absoluteTimeIndex, 24);

    auto result2 = convertBlockTimeStepToAbsoluteTimeStep(10, TimeConversionMode::DailyBlocks, 2);
    BOOST_CHECK_EQUAL(result2.block, 3);
    BOOST_CHECK_EQUAL(*result2.blockTimeIndex, 11);
    BOOST_CHECK_EQUAL(*result2.absoluteTimeIndex, 59);

    // Weekly blocks - exactly at week boundary
    auto result3 = convertBlockTimeStepToAbsoluteTimeStep(10, TimeConversionMode::WeeklyBlocks, 0);
    BOOST_CHECK_EQUAL(result3.block, 1);
    BOOST_CHECK_EQUAL(*result3.blockTimeIndex, 11);
    BOOST_CHECK_EQUAL(*result3.absoluteTimeIndex, 11);

    auto result4 = convertBlockTimeStepToAbsoluteTimeStep(0, TimeConversionMode::WeeklyBlocks, 1);
    BOOST_CHECK_EQUAL(result4.block, 2);
    BOOST_CHECK_EQUAL(*result4.blockTimeIndex, 1);
    BOOST_CHECK_EQUAL(*result4.absoluteTimeIndex, 169);

    auto result5 = convertBlockTimeStepToAbsoluteTimeStep(167, TimeConversionMode::WeeklyBlocks, 1);
    BOOST_CHECK_EQUAL(result5.block, 2);
    BOOST_CHECK_EQUAL(*result5.blockTimeIndex, 168);
    BOOST_CHECK_EQUAL(*result5.absoluteTimeIndex, 336);
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

BOOST_AUTO_TEST_CASE(BuildModelerConstraintName_OnlyTimestep)
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

BOOST_AUTO_TEST_SUITE(MockLinearProblemTests)

// Test the mock implementations
BOOST_AUTO_TEST_CASE(MockMipVariable_Interface)
{
    MockMipVariable var(42.5, MipBasisStatus::BASIC, true);

    BOOST_CHECK_EQUAL(var.solutionValue(), 42.5);
    BOOST_CHECK_EQUAL(var.getMipBasisStatus(), MipBasisStatus::BASIC);
    BOOST_CHECK_EQUAL(var.isInteger(), true);
    BOOST_CHECK_EQUAL(var.getName(), "test_var");
}

BOOST_AUTO_TEST_CASE(MockMipConstraint_Interface)
{
    MockMipConstraint constraint(MipBasisStatus::AT_LOWER_BOUND);

    BOOST_CHECK_EQUAL(constraint.getMipBasisStatus(), MipBasisStatus::AT_LOWER_BOUND);
    BOOST_CHECK_EQUAL(constraint.getName(), "test_constraint");
    BOOST_CHECK_EQUAL(constraint.getCoefficient(nullptr), 1.0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(FileSystemIntegrationTests)

BOOST_FIXTURE_TEST_CASE(FullWorkflow_CreateWriteRead, SimulationTableFileFixture)
{
    SimulationTable table;

    // Add multiple entries
    for (int i = 0; i < 5; ++i)
    {
        SimulationTableEntry entry{.block = static_cast<unsigned>(i + 1),
                                   .component = "component_" + std::to_string(i),
                                   .output = "output_" + std::to_string(i),
                                   .absolute_time_index = i * 10,
                                   .block_time_index = i * 5,
                                   .scenario_index = static_cast<unsigned>(i % 3),
                                   .value = i * 2.5,
                                   .status = static_cast<MipBasisStatus>(i % 6)};
        table.addEntry(entry);
    }

    csv_writer.writeTable(table);

    // Verify file exists and has correct name
    BOOST_CHECK(std::filesystem::exists(out_file_path));

    std::string content = readFileContent(out_file_path);

    // Check each entry
    for (int i = 0; i < 5; ++i)
    {
        std::string expectedLine = std::to_string(i + 1) + ",component_" + std::to_string(i)
                                   + ",output_" + std::to_string(i);
        BOOST_CHECK(content.find(expectedLine) != std::string::npos);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EdgeCasesAndErrorHandling)

BOOST_FIXTURE_TEST_CASE(LargeValues_HandledCorrectly, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = UINT_MAX,
                               .component = "large_comp",
                               .output = "large_var",
                               .absolute_time_index = UINT_MAX,
                               .block_time_index = UINT_MAX,
                               .scenario_index = UINT_MAX,
                               .value = std::numeric_limits<double>::max(),
                               .status = MipBasisStatus::BASIC};

    BOOST_CHECK_NO_THROW(table.addEntry(entry));
    BOOST_CHECK_NO_THROW(csv_writer.writeTable(table));
}

BOOST_FIXTURE_TEST_CASE(SpecialCharacters_InComponentNames, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp,with,commas",
                               .output = "var\"with\"quotes",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 1.0,
                               .status = MipBasisStatus::BASIC};

    table.addEntry(entry);
    csv_writer.writeTable(table);

    std::string content = readFileContent(out_file_path);
    BOOST_CHECK(content.find("comp,with,commas") != std::string::npos);
    BOOST_CHECK(content.find("var\"\"with\"\"quotes") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(ZeroValues_HandledCorrectly, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 0,
                               .component = "",
                               .output = "",
                               .absolute_time_index = 0,
                               .block_time_index = 0,
                               .scenario_index = 0,
                               .value = 0.0,
                               .status = MipBasisStatus::FREE};

    table.addEntry(entry);
    csv_writer.writeTable(table);

    std::string content = readFileContent(out_file_path);
    BOOST_CHECK(content.find("0,,") != std::string::npos);
    BOOST_CHECK(content.find(",0,Free") != std::string::npos);
}

BOOST_FIXTURE_TEST_CASE(NegativeValues_HandledCorrectly, SimulationTableFileFixture)
{
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp1",
                               .output = "var1",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = -123.456,
                               .status = MipBasisStatus::BASIC};

    table.addEntry(entry);
    csv_writer.writeTable(table);

    std::string content = readFileContent(out_file_path);
    BOOST_CHECK(content.find("-123.456") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ParameterizedTests)

// Test data for different status values
std::vector<MipBasisStatus> all_statuses = {MipBasisStatus::FREE,
                                            MipBasisStatus::AT_LOWER_BOUND,
                                            MipBasisStatus::AT_UPPER_BOUND,
                                            MipBasisStatus::FIXED_VALUE,
                                            MipBasisStatus::BASIC,
                                            MipBasisStatus::NOT_AVAILABLE};

std::vector<std::string> expected_status_strings = {"Free",
                                                    "At lower bound",
                                                    "At upper bound",
                                                    "Fixed value",
                                                    "Basic",
                                                    "None"};

BOOST_DATA_TEST_CASE(StatusToString_AllStatuses,
                     boost::unit_test::data::make(all_statuses)
                       ^ boost::unit_test::data::make(expected_status_strings),
                     status,
                     expected_string)
{
    BOOST_CHECK_EQUAL(StatusToString(status), expected_string);
}

inline std::ostream& operator<<(std::ostream& os, const TimeConversionMode& mode)
{
    switch (mode)
    {
    case TimeConversionMode::SingleBlock:
        return os << "SingleBlock";
    case TimeConversionMode::DailyBlocks:
        return os << "DailyBlocks";
    case TimeConversionMode::WeeklyBlocks:
        return os << "WeeklyBlocks";
    default:
        return os << "Unknown";
    }
}

BOOST_AUTO_TEST_SUITE_END()
