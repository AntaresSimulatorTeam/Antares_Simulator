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

#include "antares/io/outputs/SimulationTable.h"

#include "parquet_arrow_utils.h"
#include "parquet_table_writer.h"

namespace fs = std::filesystem;
using namespace Antares::IO::Outputs;
using namespace Antares::Optimisation::LinearProblemApi;

// =======================================================
// Reading a Parquet file into an Arrow table
// =======================================================
std::shared_ptr<arrow::Table> readParquet(const fs::path& file_path)
{
    std::shared_ptr<arrow::io::ReadableFile> infile;
    ARROW_THROW_ASSIGN(infile, arrow::io::ReadableFile::Open(file_path.string()));

    std::unique_ptr<parquet::arrow::FileReader> reader;
    ARROW_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

    std::shared_ptr<arrow::Table> table;
    ARROW_THROW_NOT_OK(reader->ReadTable(&table));
    return table;
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

BOOST_FIXTURE_TEST_CASE(write_SimuTable_in_parquet_then_read_it_back___reading_fits_the_writing,
                        OutputFileFixture)
{
    // Arrange
    SimulationTable table;
    SimulationTableEntry entry{.block = 1,
                               .component = "comp1",
                               .output = "var1",
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

    // --- Read back and make checks ---
    auto read_table = readParquet(file_path);
    BOOST_CHECK(read_table->num_rows() == 1);
    BOOST_CHECK_EQUAL(read_table->num_columns(), 8);

    // entry : block
    auto block = std::static_pointer_cast<arrow::Int32Array>(read_table->column(0)->chunk(0));
    BOOST_CHECK_EQUAL(block->Value(0), entry.block);

    // entry : component
    auto component = std::static_pointer_cast<arrow::StringArray>(read_table->column(1)->chunk(0));
    BOOST_CHECK_EQUAL(component->Value(0), entry.component.value());

    // entry : output
    auto output = std::static_pointer_cast<arrow::StringArray>(read_table->column(2)->chunk(0));
    BOOST_CHECK_EQUAL(output->Value(0), entry.output);

    // entry : absolute_time_index
    auto abs_time = std::static_pointer_cast<arrow::Int32Array>(read_table->column(3)->chunk(0));
    BOOST_CHECK_EQUAL(abs_time->Value(0), entry.absolute_time_index.value());

    // entry : block_time_index
    auto block_time = std::static_pointer_cast<arrow::Int32Array>(read_table->column(4)->chunk(0));
    BOOST_CHECK_EQUAL(block_time->Value(0), entry.block_time_index.value());

    // entry : scenario_index
    auto scenario = std::static_pointer_cast<arrow::Int32Array>(read_table->column(5)->chunk(0));
    BOOST_CHECK_EQUAL(scenario->Value(0), entry.scenario_index);

    // entry : value
    auto value = std::static_pointer_cast<arrow::DoubleArray>(read_table->column(6)->chunk(0));
    BOOST_CHECK_EQUAL(value->Value(0), entry.value.value());

    // entry : status
    auto status = std::static_pointer_cast<arrow::Int32Array>(read_table->column(7)->chunk(0));
    BOOST_CHECK_EQUAL(status->Value(0), static_cast<unsigned>(entry.status.value()));
}
