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
#include "columnToArrowAdapter.h"

using namespace Antares::IO::Outputs;
using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Writer
{

std::shared_ptr<arrow::DataType> arrowStorageType(const std::unique_ptr<IColumn>& column)
{
    if (dynamic_cast<StringColumn*>(column.get()))
    {
        return arrow::utf8();
    }
    else if (dynamic_cast<IntegralColumn<unsigned>*>(column.get()))
    {
        return arrow::uint32();
    }
    else if (dynamic_cast<DoubleColumn*>(column.get()))
    {
        return arrow::float64();
    }
    else if (dynamic_cast<OptionalColumn<std::string>*>(column.get()))
    {
        return arrow::utf8();
    }
    else if (dynamic_cast<OptionalColumn<unsigned>*>(column.get()))
    {
        return arrow::uint32();
    }
    else if (dynamic_cast<OptionalColumn<double>*>(column.get()))
    {
        return arrow::float64();
    }
    else if (dynamic_cast<OptionalColumn<MipBasisStatus>*>(column.get()))
    {
        return arrow::int8();
    }
    else
    {
        throw std::invalid_argument("ParquetTableWriter: column type unknown");
    }
}

ParquetTableWriter::ParquetTableWriter(std::filesystem::path& filePath):
    ITableWriter(filePath)
{
    output_file_.replace_extension(".parquet");
}

void ParquetTableWriter::writeTable(const SimulationTable& simuTable) const
{
    const auto& columns = simuTable.columns();

    // Basic validations
    if (columns.empty())
    {
        throw std::invalid_argument("ParquetTableWriter: simulation table is empty");
    }

    // ========================
    // Make the arrow table
    // ========================
    arrow::FieldVector fields;
    std::vector<std::shared_ptr<arrow::Array>> arrow_columns;
    for (const auto& column : columns)
    {
        auto columnAdapter = makeColumnAdapter(column);
        fields.push_back(columnAdapter->makeField());
        arrow_columns.push_back(columnAdapter->makeArray());
    }
    auto schema = arrow::schema(std::move(fields));
    auto table = arrow::Table::Make(schema, std::move(arrow_columns));


    // ========================
    // Write the arrow table
    // ========================
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
