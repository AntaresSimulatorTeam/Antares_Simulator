// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/solver/modeler/IWriter.h"
#include "antares/writer/i_table_writer.h"
#include "antares/writer/table_format.h"

namespace Antares::Solver
{
class FileWriter: public IWriter
{
public:
    void init(const std::string& simulationId) override;
    void writeSimulationTable(IO::Outputs::SimulationTable& SimulationTable) const override;
    explicit FileWriter(const std::filesystem::path& studyPath,
                        Writer::TableFormat fmt = Writer::TableFormat::CSV);
    const std::filesystem::path& outputPath() const override;

private:
    std::filesystem::path outputPath_;
    std::filesystem::path output_file_;
    std::string simulationId_;
    Writer::ITableWriter::Ptr writer_;
    Writer::TableFormat fmt_ = Writer::TableFormat::CSV;
};
} // namespace Antares::Solver
