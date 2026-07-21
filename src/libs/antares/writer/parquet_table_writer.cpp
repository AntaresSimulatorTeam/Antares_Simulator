// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "private/parquet_table_writer.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <utility>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/csv/writer.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>

#include "columnToArrowAdapter.h"
#include "private/parquet_arrow_utils.h"

using namespace Antares::IO::Outputs;
namespace fs = std::filesystem;

namespace Antares::Writer
{

std::shared_ptr<arrow::Table> makeArrowTable(const Antares::IO::Outputs::SimulationTable& simuTable)
{
    const auto& columns = simuTable.columns();

    arrow::FieldVector fields;
    std::vector<std::shared_ptr<arrow::Array>> arrow_columns;
    for (const auto& column: columns)
    {
        auto columnAdapter = makeColumnAdapter(*column);
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

    // --- 2. Configure CSV writer options ---
    auto csv_options = arrow::csv::WriteOptions::Defaults();
    csv_options.quoting_style = arrow::csv::QuotingStyle::None;
    csv_options.quoting_header = arrow::csv::QuotingStyle::None;

    // --- 3. Write ---
    throwOnStatusKO(arrow::csv::WriteCSV(*table, csv_options, outfile.get()));
}

ParquetTableWriter::ParquetTableWriter(const std::filesystem::path& filePath):
    ITableWriter(filePath)
{
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
