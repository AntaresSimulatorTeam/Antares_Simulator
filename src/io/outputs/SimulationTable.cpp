// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTable.h"

#include <utility>

#include "antares/io/outputs/SimulationTableEntry.h"

namespace Antares::IO::Outputs
{
SimulationTable::SimulationTable()

{
    storage_.addIntegralColumn<unsigned int>("block");
    storage_.addOptionalColumn<std::string>("component");
    storage_.addStringColumn("output");
    storage_.addOptionalColumn<unsigned int>("absolute_time_index");
    storage_.addOptionalColumn<unsigned int>("block_time_index");
    storage_.addIntegralColumn<unsigned int>("scenario_index");
    storage_.addOptionalColumn<double>("value");
    storage_.addOptionalColumn<Antares::Optimisation::LinearProblemApi::MipBasisStatus>(
      "basis_status");
}

SimulationTable::SimulationTable(SimulationTable&& other) noexcept:
    buffer_(std::move(other.buffer_)),
    storage_(std::move(other.storage_))
{
}

void SimulationTable::addEntry(const SimulationTableEntry& entry)
{
    storage_.addValue("block", entry.block);
    storage_.addValue("component", entry.component);
    storage_.addValue("output", entry.output);
    storage_.addValue("absolute_time_index", entry.absolute_time_index);
    storage_.addValue("block_time_index", entry.block_time_index);
    storage_.addValue("scenario_index", entry.scenario_index);
    storage_.addValue("value", entry.value);
    storage_.addValue("basis_status", entry.status);
}

std::string SimulationTable::headerCsvFormat() const
{
    std::ostringstream os;
    bool first = true;
    for (const auto& column: storage_.columns())
    {
        if (!first)
        {
            os << ',';
        }
        first = false;
        os << column->name();
    }
    return os.str();
}

void SimulationTable::writeHeaderToBuffer()
{
    buffer_ << headerCsvFormat() << '\n';
}

const std::vector<std::unique_ptr<IColumn>>& SimulationTable::columns() const
{
    return storage_.columns();
}

void SimulationTable::writeToBuffer()
{
    const size_t row_count = storage_.rowCount();
    const auto& columns = storage_.columns();

    for (size_t row = 0; row < row_count; ++row)
    {
        bool first = true;
        for (const auto& col: columns)
        {
            if (!first)
            {
                buffer_ << ',';
            }
            first = false;
            buffer_ << col->toString(row);
        }
        buffer_ << '\n';
    }
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
    buffer_.str("");
}

std::string SimulationTable::buffer() const
{
    return buffer_.str();
}
} // namespace Antares::IO::Outputs
