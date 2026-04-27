// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#define BOOST_TEST_MODULE testing parquet writer
#include <filesystem>

#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include "antares/io/outputs/SimulationTable.h"

#include "private/parquet_table_writer.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Outputs;
using namespace Antares::Optimisation::LinearProblemApi;

// =======================================================
// Reading a Parquet file into an Arrow table
// =======================================================
std::shared_ptr<arrow::Table> readParquet(const fs::path& file_path)
{
    std::shared_ptr<arrow::io::ReadableFile> infile;
    PARQUET_ASSIGN_OR_THROW(infile, arrow::io::ReadableFile::Open(file_path.string()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    PARQUET_ASSIGN_OR_THROW(reader, parquet::arrow::OpenFile(infile, arrow::default_memory_pool()));

    std::shared_ptr<arrow::Table> table;
    PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
    return table;
}

template<typename T>
std::shared_ptr<T> getColumn(const std::shared_ptr<arrow::Table>& table, int index)
{
    return std::static_pointer_cast<T>(table->column(index)->chunk(0));
}

struct OutputFileFixture
{
    OutputFileFixture():
        file_path(fs::temp_directory_path() / "my-output-file.parquet")
    {
        remove_if_exists();
    }

    ~OutputFileFixture()
    {
        remove_if_exists();
    }

    void remove_if_exists()
    {
        if (fs::exists(file_path))
        {
            fs::remove(file_path);
        }
    }

    fs::path file_path;
};

struct SimuTableFixture
{
    SimulationTable table;
    const std::nullopt_t none = std::nullopt;
    using enum MipBasisStatus;
};

struct LocalFixture: public OutputFileFixture, public SimuTableFixture
{
};

BOOST_FIXTURE_TEST_CASE(write_SimuTable_then_read_it_back___reading_fits, LocalFixture)
{
    // Arrange
    SimulationTableEntry entry{.block = 1,
                               .component = "component-1",
                               .output = "var-1",
                               .absolute_time_index = 1,
                               .block_time_index = 1,
                               .scenario_index = 0,
                               .value = 10.0,
                               .status = MipBasisStatus::BASIC};
    table.addEntry(entry);

    // Act
    Antares::Writer::ParquetTableWriter writer(file_path);
    writer.writeTable(table);

    // Assert
    BOOST_CHECK(fs::exists(file_path));

    // Read back the table
    auto read_table = readParquet(file_path);
    BOOST_CHECK_EQUAL(read_table->num_rows(), 1);
    BOOST_CHECK_EQUAL(read_table->num_columns(), 8);

    // entry : block
    auto block = getColumn<arrow::Int32Array>(read_table, 0);
    BOOST_CHECK_EQUAL(block->Value(0), entry.block);

    // entry : component
    auto component = getColumn<arrow::StringArray>(read_table, 1);
    BOOST_CHECK_EQUAL(component->Value(0), entry.component.value());

    // entry : output
    auto output = getColumn<arrow::StringArray>(read_table, 2);
    BOOST_CHECK_EQUAL(output->Value(0), entry.output);

    // entry : absolute_time_index
    auto abs_time = getColumn<arrow::Int32Array>(read_table, 3);
    BOOST_CHECK_EQUAL(abs_time->Value(0), entry.absolute_time_index.value());

    // entry : block_time_index
    auto block_time = getColumn<arrow::Int32Array>(read_table, 4);
    BOOST_CHECK_EQUAL(block_time->Value(0), entry.block_time_index.value());

    // entry : scenario_index
    auto scenario = getColumn<arrow::Int32Array>(read_table, 5);
    BOOST_CHECK_EQUAL(scenario->Value(0), entry.scenario_index);

    // entry : value
    auto value = getColumn<arrow::DoubleArray>(read_table, 6);
    BOOST_CHECK_EQUAL(value->Value(0), entry.value.value());

    // entry : status
    auto status = getColumn<arrow::Int32Array>(read_table, 7);
    BOOST_CHECK_EQUAL(status->Value(0), static_cast<unsigned>(entry.status.value()));
}

BOOST_FIXTURE_TEST_CASE(write_table_with_many_empty_entries_then_read_it_back___read_fits,
                        LocalFixture)
{
    // Arrange
    SimulationTableEntry entry{.block = 1,
                               .component = none,
                               .output = "var-1",
                               .absolute_time_index = none,
                               .block_time_index = none,
                               .scenario_index = 0,
                               .value = none,
                               .status = none};
    table.addEntry(entry);

    // Act
    Antares::Writer::ParquetTableWriter writer(file_path);
    writer.writeTable(table);

    // Assert
    BOOST_CHECK(fs::exists(file_path));

    // Read back the table
    auto read_table = readParquet(file_path);

    BOOST_CHECK_EQUAL(read_table->num_rows(), 1);
    BOOST_CHECK_EQUAL(read_table->num_columns(), 8);

    // entry : block
    auto block = getColumn<arrow::Int32Array>(read_table, 0);
    BOOST_CHECK_EQUAL(block->Value(0), entry.block);

    // entry : component
    auto component = getColumn<arrow::StringArray>(read_table, 1);
    BOOST_CHECK(!component->IsValid(0));

    // entry : output
    auto output = getColumn<arrow::StringArray>(read_table, 2);
    BOOST_CHECK_EQUAL(output->Value(0), entry.output);

    // entry : absolute_time_index
    auto abs_time = getColumn<arrow::Int32Array>(read_table, 3);
    BOOST_CHECK(!abs_time->IsValid(0));

    // entry : block_time_index
    auto block_time = getColumn<arrow::Int32Array>(read_table, 4);
    BOOST_CHECK(!block_time->IsValid(0));

    // entry : scenario_index
    auto scenario = getColumn<arrow::Int32Array>(read_table, 5);
    BOOST_CHECK_EQUAL(scenario->Value(0), entry.scenario_index);

    // entry : value
    auto value = getColumn<arrow::DoubleArray>(read_table, 6);
    BOOST_CHECK(!value->IsValid(0));

    // entry : status
    auto status = getColumn<arrow::Int32Array>(read_table, 7);
    BOOST_CHECK(!status->IsValid(0));
}

BOOST_FIXTURE_TEST_CASE(write_3_lines_table_then_read_it_back___read_fits, LocalFixture)
{
    // Arrange
    SimulationTableEntry line_0{1, "c1", "var-1", 5, 5, 1, none, FREE};
    SimulationTableEntry line_1{2, none, "var-2", 250, none, 4, 20., AT_LOWER_BOUND};
    SimulationTableEntry line_2{3, "c3", "var-3", none, 101, 7, 30., none};

    table.addEntry(line_0);
    table.addEntry(line_1);
    table.addEntry(line_2);

    // Act
    Antares::Writer::ParquetTableWriter writer(file_path);
    writer.writeTable(table);

    // Assert
    BOOST_CHECK(fs::exists(file_path));

    // Read back the table
    auto read_table = readParquet(file_path);

    BOOST_CHECK_EQUAL(read_table->num_rows(), 3);
    BOOST_CHECK_EQUAL(read_table->num_columns(), 8);

    // column 0 (block):
    auto col_0 = getColumn<arrow::Int32Array>(read_table, 0);
    BOOST_CHECK_EQUAL(col_0->Value(0), line_0.block);
    BOOST_CHECK_EQUAL(col_0->Value(1), line_1.block);
    BOOST_CHECK_EQUAL(col_0->Value(2), line_2.block);

    // column 1 (component):
    auto col_1 = getColumn<arrow::StringArray>(read_table, 1);
    BOOST_CHECK_EQUAL(col_1->Value(0), line_0.component.value());
    BOOST_CHECK(!col_1->IsValid(1));
    BOOST_CHECK_EQUAL(col_1->Value(2), line_2.component.value());

    // column 2 (output) :
    auto col_2 = getColumn<arrow::StringArray>(read_table, 2);
    BOOST_CHECK_EQUAL(col_2->Value(0), line_0.output);
    BOOST_CHECK_EQUAL(col_2->Value(1), line_1.output);
    BOOST_CHECK_EQUAL(col_2->Value(2), line_2.output);

    // column 3 (absolute_time_index):
    auto col_3 = getColumn<arrow::Int32Array>(read_table, 3);
    BOOST_CHECK_EQUAL(col_3->Value(0), line_0.absolute_time_index.value());
    BOOST_CHECK_EQUAL(col_3->Value(1), line_1.absolute_time_index.value());
    BOOST_CHECK(!col_3->IsValid(2));

    // column 4 (block_time_index):
    auto col_4 = getColumn<arrow::Int32Array>(read_table, 4);
    BOOST_CHECK_EQUAL(col_4->Value(0), line_0.block_time_index.value());
    BOOST_CHECK(!col_4->IsValid(1));
    BOOST_CHECK_EQUAL(col_4->Value(2), line_2.block_time_index.value());

    // column 5 (scenario_index):
    auto col_5 = getColumn<arrow::Int32Array>(read_table, 5);
    BOOST_CHECK_EQUAL(col_5->Value(0), line_0.scenario_index);
    BOOST_CHECK_EQUAL(col_5->Value(1), line_1.scenario_index);
    BOOST_CHECK_EQUAL(col_5->Value(2), line_2.scenario_index);

    // column 6 (value):
    auto col_6 = getColumn<arrow::DoubleArray>(read_table, 6);
    BOOST_CHECK(!col_6->IsValid(0));
    BOOST_CHECK_EQUAL(col_6->Value(1), line_1.value.value());
    BOOST_CHECK_EQUAL(col_6->Value(2), line_2.value.value());

    // column 7 (status):
    auto col_7 = getColumn<arrow::Int32Array>(read_table, 7);
    BOOST_CHECK_EQUAL(col_7->Value(0), (unsigned)line_0.status.value());
    BOOST_CHECK_EQUAL(col_7->Value(1), (unsigned)line_1.status.value());
    BOOST_CHECK(!col_7->IsValid(2));
}
