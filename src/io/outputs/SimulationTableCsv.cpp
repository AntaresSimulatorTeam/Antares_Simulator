/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/io/outputs/SimulationTableCsv.h"

#include "antares/io/outputs/SimulationTableEntry.h"

SimulationTableCsv::SimulationTableCsv()

{
    storage_.addIntColumn("block");
    storage_.addStringColumn("component");
    storage_.addStringColumn("output");
    storage_.addOptionalColumn<unsigned int>("absolute_time_index");
    storage_.addOptionalColumn<unsigned int>("block_time_index");
    storage_.addOptionalColumn<unsigned int>("scenario_index");
    storage_.addOptionalColumn<double>("value");
    storage_.addStringColumn("basis_status");
    writeHeader();
}

void SimulationTableCsv::addEntry(SimulationTableEntry entry)
{
    storage_.addValue("block", entry.block);
    storage_.addValue("component", entry.component);
    storage_.addValue("output", entry.output);
    storage_.addValue("absolute_time_index", entry.absolute_time_index);
    storage_.addValue("block_time_index", entry.block_time_index);
    storage_.addValue("scenario_index", entry.scenario_index);
    storage_.addValue("value", *entry.value);
    storage_.addValue("basis_status", StatusToString(entry.status));
}

void SimulationTableCsv::writeHeader()
{
    bool first = true;
    for (const auto& col_name: storage_.columnOrder())
    {
        if (!first)
        {
            buffer_ << ',';
        }
        first = false;
        buffer_ << col_name;
    }
    buffer_ << '\n';
}

const std::string NONE = "None";

template<typename T>
std::string extractFromOptional(const std::optional<T>& option)
{
    return option.has_value() ? std::to_string(option.value()) : NONE;
}

void SimulationTableCsv::write()
{
    // for (const auto& [block,
    //                   component,
    //                   output,
    //                   absolute_time_index,
    //                   block_time_index,
    //                   scenario_index,
    //                   value,
    //                   status]: entries_)
    // {
    //     buffer_ << block << ',' << component << ',' << output << ','
    //             << extractFromOptional(absolute_time_index) << ','
    //             << extractFromOptional(block_time_index) << ','
    //             << extractFromOptional(scenario_index) << ',' << extractFromOptional(value) <<
    //             ','
    //             << StatusToString(status) << '\n';
    // }

    const size_t row_count = storage_.rowCount();
    const auto& columns = storage_.columnOrder();

    for (size_t row = 0; row < row_count; ++row)
    {
        bool first = true;
        for (const auto& col_name: columns)
        {
            if (!first)
            {
                buffer_ << ',';
            }
            first = false;
            buffer_ << storage_.getColumn(col_name).toString(row);
        }
        buffer_ << '\n';
    }
}

void SimulationTableCsv::clear()
{
    entries_.clear();
    buffer_.str("");
}

//
// const std::string& SimulationTableCsv::buffer() const
// {
//     return buffer_.str();
// }

std::string SimulationTableCsv::buffer() const
{
    return buffer_.str();
}
