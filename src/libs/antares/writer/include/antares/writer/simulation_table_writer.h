// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>

#include "table_format.h"

namespace Antares::IO::Outputs
{
class SimulationTable;
} // namespace Antares::IO::Outputs

namespace Antares::Writer
{

class SimulationTableWriter final
{
public:
    SimulationTableWriter(const std::filesystem::path& filePath, TableFormat tableFormat);
    void writeTable(const IO::Outputs::SimulationTable& simuTable) const;

    [[nodiscard]] const std::filesystem::path& outputFile() const
    {
        return output_file_;
    }

private:
    void writeParquet(const std::filesystem::path& file_path,
                      const IO::Outputs::SimulationTable& simuTable) const;
    void writeCsv(const std::filesystem::path& file_path,
                  const IO::Outputs::SimulationTable& simuTable) const;

    std::filesystem::path output_file_;
    TableFormat table_format_;
};

/// Writes a single (CSV or Parquet) file incrementally: each append() adds the
/// current rows of the table to the file (one Parquet row group per call), so
/// the caller can clear() its in-memory table between calls.
/// The file is created lazily by the first non-empty append(), so a stream that
/// never receives any row produces no file. close() (also called by the
/// destructor) finalises the file (Parquet footer).
class StreamingSimulationTableWriter final
{
public:
    StreamingSimulationTableWriter(const std::filesystem::path& filePath, TableFormat tableFormat);
    ~StreamingSimulationTableWriter();
    StreamingSimulationTableWriter(const StreamingSimulationTableWriter&) = delete;
    StreamingSimulationTableWriter& operator=(const StreamingSimulationTableWriter&) = delete;
    StreamingSimulationTableWriter(StreamingSimulationTableWriter&& other);
    StreamingSimulationTableWriter& operator=(StreamingSimulationTableWriter&& other);

    /// Appends the rows currently stored in the table. No-op if the table has no row.
    void append(const IO::Outputs::SimulationTable& simuTable);
    void close();

    [[nodiscard]] const std::filesystem::path& outputFile() const
    {
        return output_file_;
    }

private:
    struct Impl;
    std::filesystem::path output_file_;
    TableFormat table_format_;
    std::unique_ptr<Impl> impl_;
};

} // namespace Antares::Writer
