// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "antares/io/outputs/OptimisationsSimulationTable.h"
#include "antares/io/outputs/SimulationTable.h"

#include "simulation_table_writer.h"
#include "table_format.h"

namespace Antares::Writer
{
class LegacySimulationTablesWriter
{
public:
    LegacySimulationTablesWriter(const std::filesystem::path& folder,
                                 const unsigned year,
                                 TableFormat tableFormat = TableFormat::CSV);

    /// One-shot write of everything in `tables` (flush + finish). Tables are
    /// left untouched.
    void write(const IO::Outputs::OptimisationsSimulationTable& tables);

    /// Streaming API, to keep the in-memory tables small: appends the rows
    /// currently held by each stage of `tables` to the stage's file for this
    /// year, then clears them. Stages with no row are skipped (their file is
    /// created only once they have content). Can be called any number of times
    /// (e.g. once per week), then finish() must close the files.
    void flush(const IO::Outputs::OptimisationsSimulationTable& tables);

    /// Flushes the remaining rows and closes all files of the year.
    void finish(const IO::Outputs::OptimisationsSimulationTable& tables);

private:
    StreamingSimulationTableWriter& streamFor(IO::Outputs::Stage stage);
    std::map<IO::Outputs::Stage, StreamingSimulationTableWriter> streams_;
    unsigned year_ = 0;
    TableFormat tableFormat_;
    const std::filesystem::path folder_;
};
} // namespace Antares::Writer
