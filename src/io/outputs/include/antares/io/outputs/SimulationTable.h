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
    const std::vector<std::shared_ptr<IColumn>>& columns() const;
    size_t rowCount() const;
    void clear();
    std::vector<std::vector<std::string>> storageIntoRows() const;

private:
    ColumnBasedStorage storage_;
    std::shared_ptr<IntegralColumn> block_;
    std::shared_ptr<OptionalColumn<std::string>> component_;
    std::shared_ptr<StringColumn> output_;
    std::shared_ptr<OptionalColumn<unsigned int>> absolute_time_index_;
    std::shared_ptr<OptionalColumn<unsigned int>> block_time_index_;
    std::shared_ptr<IntegralColumn> scenario_index_;
    std::shared_ptr<OptionalColumn<double>> value_;
    std::shared_ptr<OptionalColumn<Antares::Optimisation::LinearProblemApi::MipBasisStatus>>
      basis_status_;
};
} // namespace Antares::IO::Outputs
