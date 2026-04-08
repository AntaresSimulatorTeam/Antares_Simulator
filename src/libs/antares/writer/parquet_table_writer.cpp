// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "parquet_table_writer.h"

#include <filesystem>
#include <sstream>
#include <stdexcept>

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/writer.h>

using namespace Antares::IO::Outputs;

namespace Antares::Writer
{

ParquetTableWriter::ParquetTableWriter(std::filesystem::path& filePath):
    ITableWriter(filePath)
{
    output_file_.replace_extension(".parquet");
}

void ParquetTableWriter::writeTable(const SimulationTable& simuTable) const
{
    std::vector<std::string> colNames = simuTable.columnNames();
    const auto& columns = simuTable.columns();

    // Basic validation
    if (colNames.empty())
    {
        throw std::invalid_argument("ParquetTableWriter: header is empty");
    }

    const size_t ncols = colNames.size();

    // Schema: all columns as UTF8 strings
    arrow::FieldVector fields;
    fields.reserve(static_cast<int>(ncols));
    for (const auto& name: colNames)
    {
        // Duplicate names are allowed by Arrow but discouraged; keep as-is.
        fields.push_back(arrow::field(name, arrow::utf8()));
    }
    auto schema = arrow::schema(std::move(fields));

    // Build columns using StringBuilder
    std::vector<std::shared_ptr<arrow::Array>> arrow_columns;
    arrow_columns.reserve(ncols);

    arrow::StringBuilder builder;

    size_t col_index = 0;
    for (const auto& column: columns)
    {
        for (size_t row = 0; row < column->size(); ++row)
        {
            const std::string& cell = column->toString(row);
            auto st = builder.Append(cell);
            if (!st.ok())
            {
                std::ostringstream oss;
                oss << "ParquetTableWriter: failed to append cell in column " << colNames[col_index]
                    << ": " << st.ToString();
                throw std::runtime_error(oss.str());
            }
        }

        std::shared_ptr<arrow::Array> array;
        auto st_fin = builder.Finish(&array);
        if (!st_fin.ok())
        {
            std::ostringstream oss;
            oss << "ParquetTableWriter: failed to finalize column " << colNames[col_index] << ": "
                << st_fin.ToString();
            throw std::runtime_error(oss.str());
        }
        arrow_columns.push_back(std::move(array));
        col_index++;
    }

    // Make table
    auto table = arrow::Table::Make(schema, std::move(arrow_columns), static_cast<int64_t>(ncols));

    // Open output file
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    auto st_out = arrow::io::FileOutputStream::Open(output_file_.string());
    if (!st_out.ok())
    {
        std::ostringstream oss;
        oss << "ParquetTableWriter: cannot open output file '" << output_file_.string()
            << "': " << st_out.status().ToString();
        throw std::runtime_error(oss.str());
    }
    outfile = *st_out;

    // Parquet write with default properties
    parquet::WriterProperties::Builder props_builder;
    auto props = props_builder.build();

    // Chunk size: write all at once or in default 1024; use total rows if small
    const int64_t chunk_size = std::max<int64_t>(1, std::min<int64_t>(1024, table->num_rows()));

    auto st_write = parquet::arrow::WriteTable(*table,
                                               arrow::default_memory_pool(),
                                               outfile,
                                               chunk_size,
                                               props);
    if (!st_write.ok())
    {
        std::ostringstream oss;
        oss << "ParquetTableWriter: failed to write table: " << st_write.ToString();
        throw std::runtime_error(oss.str());
    }
}

} // namespace Antares::Writer
