// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include "antares/io/outputs/storage.h"

#include "SimulationTableEntry.h"

namespace Antares::IO::Outputs
{
class SimulationTable
{
public:
    SimulationTable();
    SimulationTable(SimulationTable&& other) noexcept;
    void addEntry(const SimulationTableEntry& entry);
    [[nodiscard]] std::string headerCsvFormat() const;
    const std::vector<std::unique_ptr<IColumn>>& columns() const;
    void writeToBuffer();
    void clear();

    // const std::string& buffer() const;
    [[nodiscard]] std::string buffer() const;

    // gp : never called in production code, only from tests
    // gp : should be moved as a free function in tests
    void writeHeaderToBuffer();

    std::vector<std::vector<std::string>> storageIntoRows() const;

private:
    std::ostringstream buffer_;
    ColumnBasedStorage storage_;
};
} // namespace Antares::IO::Outputs
