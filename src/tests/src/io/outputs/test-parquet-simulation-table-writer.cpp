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

    fs::remove(file_path);
}
