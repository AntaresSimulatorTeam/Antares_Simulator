// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/io/outputs/SimulationTable.h"

namespace Antares::Solver
{
// TODO this class needs to be revamped
class IWriter
{
public:
    virtual ~IWriter() = default;
    virtual void init(const std::string& simulationId) = 0;
    virtual void writeSimulationTable(IO::Outputs::SimulationTable& simuTable) const = 0;
    virtual const std::filesystem::path& outputPath() const = 0;
};
} // namespace Antares::Solver
