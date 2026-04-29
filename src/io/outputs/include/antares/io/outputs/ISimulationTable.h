// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "SimulationTableEntry.h"

namespace Antares::IO::Outputs
{
class ISimulationTable
{
public:
    virtual ~ISimulationTable() = default;
    virtual void addEntry(const SimulationTableEntry& entry) = 0;
    virtual void clear() = 0;
    [[nodiscard]] virtual std::string buffer() const = 0;
    /// Write the table to the given file path, using the concrete export format
    virtual void write() = 0;
};
} // namespace Antares::IO::Outputs
