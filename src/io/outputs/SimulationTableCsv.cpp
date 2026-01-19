// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/outputs/SimulationTableCsv.h"

#include "antares/io/outputs/SimulationTableEntry.h"

SimulationTableCsv::SimulationTableCsv()

{
    storage_.addIntegralColumn<unsigned int>("block");
    storage_.addOptionalColumn<std::string>("component");
    storage_.addStringColumn("output");
    storage_.addOptionalColumn<unsigned int>("absolute_time_index");
    storage_.addOptionalColumn<unsigned int>("block_time_index");
    storage_.addOptionalColumn<unsigned int>("scenario_index");
    storage_.addOptionalColumn<double>("value");
    storage_.addOptionalColumn<Antares::Optimisation::LinearProblemApi::MipBasisStatus>(
      "basis_status");
}

void SimulationTableCsv::addEntry(const SimulationTableEntry& entry)
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

std::string SimulationTableCsv::getHeader() const
{
    std::ostringstream os;
    bool first = true;
    for (const auto& col_name: storage_.columnNames())
    {
        if (!first)
        {
            os << ',';
        }
        first = false;
        os << col_name;
    }
    return os.str();
}

void SimulationTableCsv::writeHeader()
{
    buffer_ << getHeader() << '\n';
}

const std::string NONE = "None";

void SimulationTableCsv::write()
{
    const size_t row_count = storage_.rowCount();
    // const auto& nameToIndex = storage_.columnsNameToIndex();
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

void SimulationTableCsv::clear()
{
    storage_.clear();
    buffer_.str("");
}

std::string SimulationTableCsv::buffer() const
{
    return buffer_.str();
}
