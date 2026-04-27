// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/parquet_table_writer.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>

#include "private/columnToArrowAdapter.h"
#include "private/parquet_arrow_utils.h"

using namespace Antares::IO::Outputs;
namespace fs = std::filesystem;

namespace Antares::Writer
{

std::shared_ptr<arrow::Table> makeArrowTable(const Antares::IO::Outputs::SimulationTable& simuTable)
{
    const auto& columns = simuTable.columns();

    // Schema: all columns as UTF8 strings
    arrow::FieldVector fields;
    std::vector<std::shared_ptr<arrow::Array>> arrow_columns;
    for (const auto& column: columns)
    {
        auto columnAdapter = makeColumnAdapter(column);
        fields.push_back(columnAdapter->makeField());
        arrow_columns.push_back(columnAdapter->makeArray());
    }
    auto schema = arrow::schema(std::move(fields));
    return arrow::Table::Make(schema, std::move(arrow_columns));
}

void writeParquet(const std::shared_ptr<arrow::Table>& table, const fs::path& file_path)
{
    // --- 1. Open output file ---
    auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(file_path.string()));

    // --- 2. Configure Parquet writer ---
    auto writer_props = parquet::WriterProperties::Builder()
                          .compression(arrow::Compression::SNAPPY)
                          ->version(parquet::ParquetVersion::PARQUET_2_6)
                          ->build();

    auto arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();

    // --- 3. Write ---
    throwOnStatusKO(parquet::arrow::WriteTable(*table,
                                               arrow::default_memory_pool(),
                                               outfile,
                                               /*chunk_size=*/1024,
                                               writer_props,
                                               arrow_props));
}

ParquetTableWriter::ParquetTableWriter(std::filesystem::path& filePath):
    ITableWriter(filePath)
{
    output_file_.replace_extension(".parquet");
}

void ParquetTableWriter::writeTable(const SimulationTable& simuTable) const
{
    // Basic validations
    if (simuTable.columns().empty())
    {
        throw std::invalid_argument("ParquetTableWriter: simulation table is empty");
    }

    auto table = makeArrowTable(simuTable);
    writeParquet(table, output_file_);
}

} // namespace Antares::Writer
