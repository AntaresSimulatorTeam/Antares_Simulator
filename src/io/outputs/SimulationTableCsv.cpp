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

#include <fstream>

#include "antares/io/outputs/SimulationTableEntry.h"

SimulationTableCsv::SimulationTableCsv()

{
    writeHeader();
}

void SimulationTableCsv::addEntry(SimulationTableEntry entry)
{
    entries_.emplace_back(std::move(entry));
}

void SimulationTableCsv::writeHeader()
{
    buffer_ << "block,component,output,"
               "absolute_time_index,block_time_index,scenario_index,value,basis_status\n";
}

void SimulationTableCsv::write()
{
    for (const auto& [block,
                      component,
                      output,
                      absolute_time_index,
                      block_time_index,
                      scenario_index,
                      value,
                      status]: entries_)
    {
        buffer_ << block << ',' << component << ',' << output << ','
                << (absolute_time_index ? std::to_string(*absolute_time_index) : "None") << ','
                << (block_time_index ? std::to_string(*block_time_index) : "None") << ','
                << (scenario_index ? std::to_string(*scenario_index) : "None") << ',' << value
                << ',' << StatusToString(status) << '\n';
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
