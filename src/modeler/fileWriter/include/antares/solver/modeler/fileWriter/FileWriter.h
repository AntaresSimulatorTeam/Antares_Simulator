// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/solver/modeler/IWriter.h"

namespace Antares::Solver
{
class FileWriter: public IWriter
{
public:
    void init(const std::string& simulationId) override;
    void writeSimulationTable(IO::Outputs::SimulationTable& SimulationTable) const override;
    explicit FileWriter(const std::filesystem::path& studyPath, bool parquetFormatRequired = false);
    const std::filesystem::path& outputPath() const;

private:
    std::filesystem::path outputPath_;
    std::filesystem::path output_file_;
    std::string simulationId_;
    bool parquetFormatRequired_ = false;
};
} // namespace Antares::Solver
