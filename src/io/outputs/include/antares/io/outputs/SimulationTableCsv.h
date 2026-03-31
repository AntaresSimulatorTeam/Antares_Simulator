// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>
#include <vector>

#include "antares/io/outputs/storage.h"

#include "ISimulationTable.h"

namespace Antares::IO::Outputs
{
class SimulationTableCsv: public ISimulationTable
{
public:
    SimulationTableCsv();
    void addEntry(const SimulationTableEntry& entry) override;
    [[nodiscard]] std::string headerCsvFormat() const;
    std::vector<std::string> rawHeader() const;
    void write() override;
    void clear() override;

    // const std::string& buffer() const;
    [[nodiscard]] std::string buffer() const override;
    void writeHeaderToBuffer();

protected:
    std::vector<std::vector<std::string>> storageIntoRows() const;

private:
    std::ostringstream buffer_;
    ColumnBasedStorage storage_;
};
} // namespace Antares::IO::Outputs
