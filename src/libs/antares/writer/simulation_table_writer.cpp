// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/writer/simulation_table_writer.h"

#include <filesystem>
#include <stdexcept>
#include <utility>

#include "antares/io/outputs/SimulationTable.h"
#include "antares/logs/logs.h"

// Arrow / Parquet
#include <arrow/api.h>
#include <arrow/csv/writer.h>
#include <arrow/io/api.h>
#include <arrow/ipc/writer.h>
#include <parquet/arrow/writer.h>

#include "columnToArrowAdapter.h"
#include "private/parquet_arrow_utils.h"

namespace fs = std::filesystem;

namespace Antares::Writer
{

namespace
{

std::shared_ptr<arrow::Table> makeArrowTable(const IO::Outputs::SimulationTable& simuTable)
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

[[nodiscard]] std::filesystem::path replaceExtension(const std::filesystem::path& filePath,
                                                     TableFormat tableFormat)
{
    auto adjustedPath = filePath;
    adjustedPath.replace_extension(tableFormat == TableFormat::CSV ? ".csv" : ".parquet");
    return adjustedPath;
}

std::shared_ptr<parquet::WriterProperties> parquetWriterProperties()
{
    return parquet::WriterProperties::Builder()
      .compression(arrow::Compression::ZSTD)
      ->compression_level(2)
      ->data_pagesize(2 * 1024 * 1024)
      ->write_batch_size(256 * 1024)
      ->version(parquet::ParquetVersion::PARQUET_2_6)
      ->build();
}

arrow::csv::WriteOptions csvWriteOptions()
{
    auto csv_options = arrow::csv::WriteOptions::Defaults();
    csv_options.quoting_style = arrow::csv::QuotingStyle::None;
    csv_options.quoting_header = arrow::csv::QuotingStyle::None;
    csv_options.null_string = "None";
    return csv_options;
}

constexpr int64_t parquetChunkSize = 1024 * 1024;

void createParentDirectories(const fs::path& file)
{
    auto p = file.parent_path();
    if (!p.empty())
    {
        std::error_code ec;
        std::filesystem::create_directories(p, ec);
    }
}

} // anonymous namespace

SimulationTableWriter::SimulationTableWriter(const std::filesystem::path& filePath,
                                             TableFormat tableFormat):
    output_file_(replaceExtension(filePath, tableFormat)),
    table_format_(tableFormat)
{
    createParentDirectories(output_file_);
}

void SimulationTableWriter::writeParquet(const fs::path& file_path,
                                         const IO::Outputs::SimulationTable& simuTable) const
{
    auto table = makeArrowTable(simuTable);

    // Open output file
    auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(file_path.string()));

    // Configure Parquet writer properties
    auto writer_props = parquetWriterProperties();

    auto arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();

    // Write as Parquet
    throwOnStatusKO(parquet::arrow::WriteTable(*table,
                                               arrow::default_memory_pool(),
                                               outfile,
                                               parquetChunkSize,
                                               writer_props,
                                               arrow_props));
}

void SimulationTableWriter::writeCsv(const fs::path& file_path,
                                     const IO::Outputs::SimulationTable& simuTable) const
{
    auto table = makeArrowTable(simuTable);

    // Configure CSV writer options
    auto csv_options = csvWriteOptions();

    // Open output file
    auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(file_path.string()));

    // Write as CSV
    throwOnStatusKO(arrow::csv::WriteCSV(*table, csv_options, outfile.get()));
}

void SimulationTableWriter::writeTable(const IO::Outputs::SimulationTable& simuTable) const
{
    if (simuTable.columns().empty())
    {
        throw std::invalid_argument("SimulationTableWriter: simulation table is empty");
    }

    switch (table_format_)
    {
    case TableFormat::Parquet:
        writeParquet(output_file_, simuTable);
        break;
    case TableFormat::CSV:
        writeCsv(output_file_, simuTable);
        break;
    default:
        throw std::invalid_argument("SimulationTableWriter: unknown table format");
    }
}

// ---------------------------------------------------------------------------
// StreamingSimulationTableWriter
// ---------------------------------------------------------------------------

struct StreamingSimulationTableWriter::Impl
{
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    // Parquet
    std::unique_ptr<parquet::arrow::FileWriter> parquetWriter;
    // CSV
    std::shared_ptr<arrow::ipc::RecordBatchWriter> csvWriter;
};

StreamingSimulationTableWriter::StreamingSimulationTableWriter(const fs::path& filePath,
                                                               TableFormat tableFormat):
    output_file_(replaceExtension(filePath, tableFormat)),
    table_format_(tableFormat)
{
    createParentDirectories(output_file_);
}

StreamingSimulationTableWriter::~StreamingSimulationTableWriter()
{
    try
    {
        close();
    }
    catch (const std::exception& e)
    {
        logs.error() << "Failed to finalize simulation table file '" << output_file_
                     << "': " << e.what();
    }
    catch (...)
    {
        logs.error() << "Failed to finalize simulation table file '" << output_file_
                     << "': unknown exception";
    }
}

StreamingSimulationTableWriter::StreamingSimulationTableWriter(StreamingSimulationTableWriter&&)
  = default;
StreamingSimulationTableWriter& StreamingSimulationTableWriter::operator=(
  StreamingSimulationTableWriter&&)
  = default;

void StreamingSimulationTableWriter::append(const IO::Outputs::SimulationTable& simuTable)
{
    if (simuTable.columns().empty())
    {
        throw std::invalid_argument("StreamingSimulationTableWriter: simulation table is empty");
    }
    if (simuTable.rowCount() == 0)
    {
        return;
    }

    auto table = makeArrowTable(simuTable);

    if (!impl_)
    {
        auto outfile = throwOnResultKO(arrow::io::FileOutputStream::Open(output_file_.string()));
        impl_ = std::make_unique<Impl>();
        impl_->outfile = std::move(outfile);
        switch (table_format_)
        {
        case TableFormat::Parquet:
        {
            auto arrow_props = parquet::ArrowWriterProperties::Builder().store_schema()->build();
            impl_->parquetWriter = throwOnResultKO(
              parquet::arrow::FileWriter::Open(*table->schema(),
                                               arrow::default_memory_pool(),
                                               impl_->outfile,
                                               parquetWriterProperties(),
                                               arrow_props));
            break;
        }
        case TableFormat::CSV:
            impl_->csvWriter = throwOnResultKO(
              arrow::csv::MakeCSVWriter(impl_->outfile, table->schema(), csvWriteOptions()));
            break;
        default:
            throw std::invalid_argument("StreamingSimulationTableWriter: unknown table format");
        }
    }

    if (impl_->parquetWriter)
    {
        // One row group per call
        throwOnStatusKO(impl_->parquetWriter->WriteTable(*table, parquetChunkSize));
    }
    else
    {
        throwOnStatusKO(impl_->csvWriter->WriteTable(*table));
    }
}

void StreamingSimulationTableWriter::close()
{
    if (!impl_)
    {
        return;
    }
    auto impl = std::move(impl_);
    if (impl->parquetWriter)
    {
        throwOnStatusKO(impl->parquetWriter->Close());
    }
    if (impl->csvWriter)
    {
        throwOnStatusKO(impl->csvWriter->Close());
    }
    throwOnStatusKO(impl->outfile->Close());
}

} // namespace Antares::Writer
