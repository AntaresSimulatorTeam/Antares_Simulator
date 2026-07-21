// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/writer/parquet_table_writer.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "antares/io/outputs/SimulationTable.h"

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

std::shared_ptr<arrow::Table> makeArrowTable(const SimulationTable& simuTable)
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

ParquetTableWriter::ParquetTableWriter(const std::filesystem::path& filePath,
                                       TableFormat tableFormat):
    output_file_(filePath),
    table_format_(tableFormat)
{
    auto p = output_file_.parent_path();
    if (!p.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(p, ec);
    }
}

void ParquetTableWriter::writeParquet(const fs::path& file_path,
                                      const SimulationTable& simuTable) const
{
    if (simuTable.columns().empty())
    {
        throw std::invalid_argument("ParquetTableWriter: simulation table is empty");
    }

    auto table = makeArrowTable(simuTable);

    // Open output file
    auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(file_path.string()));

    // Write as Parquet
    throwOnStatusKO(parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile));
}

void ParquetTableWriter::writeCsv(const fs::path& file_path,
                                  const SimulationTable& simuTable) const
{
    if (simuTable.columns().empty())
    {
        throw std::invalid_argument("ParquetTableWriter: simulation table is empty");
    }

    auto table = makeArrowTable(simuTable);

    // Configure CSV writer options
    auto csv_options = arrow::csv::WriteOptions::Defaults();
    csv_options.quoting_style = arrow::csv::QuotingStyle::None;
    csv_options.quoting_header = arrow::csv::QuotingStyle::None;

    // Open output file
    auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(file_path.string()));

    // Write as CSV
    throwOnStatusKO(arrow::csv::WriteCSV(*table, csv_options, outfile.get()));
}

void ParquetTableWriter::writeTable(const SimulationTable& simuTable) const
{
    switch (table_format_)
    {
        case TableFormat::Parquet:
            writeParquet(output_file_, simuTable);
            break;
        case TableFormat::CSV:
            writeCsv(output_file_, simuTable);
            break;
    }
}

} // namespace Antares::Writer
