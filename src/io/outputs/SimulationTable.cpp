// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTable.h"

#include <utility>

#include "antares/io/outputs/SimulationTableEntry.h"

namespace Antares::IO::Outputs
{
SimulationTable::SimulationTable()

{
    block_ = storage_.addIntegralColumn("block");
    component_ = storage_.addOptionalColumn<std::string>("component");
    output_ = storage_.addStringColumn("output");
    absolute_time_index_ = storage_.addOptionalColumn<unsigned int>("absolute_time_index");
    block_time_index_ = storage_.addOptionalColumn<unsigned int>("block_time_index");
    scenario_index_ = storage_.addIntegralColumn("scenario_index");
    value_ = storage_.addOptionalColumn<double>("value");
    basis_status_ = storage_
                      .addOptionalColumn<Antares::Optimisation::LinearProblemApi::MipBasisStatus>(
                        "basis_status");
}

SimulationTable::SimulationTable(SimulationTable&& other) noexcept = default;

void SimulationTable::addEntry(const SimulationTableEntry& entry)
{
    block_->add(entry.block);
    component_->add(entry.component);
    output_->add(entry.output);
    absolute_time_index_->add(entry.absolute_time_index);
    block_time_index_->add(entry.block_time_index);
    scenario_index_->add(entry.scenario_index);
    value_->add(entry.value);
    basis_status_->add(entry.status);
}

const std::vector<std::shared_ptr<IColumn>>& SimulationTable::columns() const
{
    return storage_.columns();
}

size_t SimulationTable::rowCount() const
{
    return storage_.rowCount();
}

std::vector<std::vector<std::string>> SimulationTable::storageIntoRows() const
{
    std::vector<std::vector<std::string>> rows; // to return
    const size_t row_count = storage_.rowCount();
    const auto& columns = storage_.columns();

    rows.clear();
    rows.resize(row_count);
    for (size_t row_index = 0; row_index < row_count; ++row_index)
    {
        auto& row = rows[row_index];
        row.reserve(columns.size());
        for (const auto& column: columns)
        {
            row.push_back(column->toString(row_index));
        }
    }
    return rows;
}

void SimulationTable::clear()
{
    storage_.clear();
}
} // namespace Antares::IO::Outputs
