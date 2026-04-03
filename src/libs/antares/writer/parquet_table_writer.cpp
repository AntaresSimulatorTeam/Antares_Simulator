/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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

// gp : not responsibility of a particular writer : we have the same check for csv writer
static void ensureParentDir(const std::filesystem::path& file)
{
    auto p = file.parent_path();
    if (!p.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(p, ec);
        // Best effort; ignore error as writer will fail on open if directory truly invalid
    }
}

void ParquetTableWriter::writeTable(const std::filesystem::path& filePath,
                                    const SimulationTable& simuTable)
{
    std::vector<std::string> colNames = simuTable.columnNames();
    const auto& columns = simuTable.columns();

    // Basic validation
    if (colNames.empty())
    {
        throw std::invalid_argument("ParquetTableWriter: header is empty");
    }

    const size_t ncols = colNames.size();

    ensureParentDir(filePath);

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
    auto st_out = arrow::io::FileOutputStream::Open(filePath.string());
    if (!st_out.ok())
    {
        std::ostringstream oss;
        oss << "ParquetTableWriter: cannot open output file '" << filePath.string()
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
