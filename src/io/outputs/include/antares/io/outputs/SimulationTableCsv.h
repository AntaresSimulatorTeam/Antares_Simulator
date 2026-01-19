// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/io/outputs/storage.h"

#include "ISimulationTable.h"

namespace Antares::IO::Outputs
{
class SimulationTableCsv: public ISimulationTable
{
public:
    SimulationTableCsv();
    void addEntry(const SimulationTableEntry& entry) override;
    [[nodiscard]] std::string getHeader() const;
    void write() override;
    void clear() override;

    // const std::string& buffer() const;
    [[nodiscard]] std::string buffer() const override;
    void writeHeader();

private:
    std::ostringstream buffer_;
    ColumnBasedStorage storage_;
};
} // namespace Antares::IO::Outputs
