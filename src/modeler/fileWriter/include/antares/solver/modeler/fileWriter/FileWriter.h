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
    void writeSimulationTable(IO::Outputs::SimulationTableCsv& SimulationTable) const override;
    explicit FileWriter(std::filesystem::path path, bool parquetFormatRequired = false);
    const std::filesystem::path& outputPath() const;

private:
    const std::filesystem::path studyPath_;
    std::filesystem::path outputPath_;
    bool parquetFormatRequired_ = false;
    std::string simulationId_;
};
} // namespace Antares::Solver
