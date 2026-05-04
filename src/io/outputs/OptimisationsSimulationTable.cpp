// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"

#include <utility>

using namespace Antares::IO::Outputs;

void OptimisationsSimulationTable::clear()
{
    firstOptimBuffer_.clear();
    secondOptimBuffer_.clear();
}

std::pair<std::string, std::string> OptimisationsSimulationTable::moveBuffers()
{
    auto result = std::pair<std::string, std::string>{std::move(firstOptimBuffer_),
                                                      std::move(secondOptimBuffer_)};
    clear();
    return result;
}

void OptimisationsSimulationTable::write()
{
    firstOptimSimulationTable_.writeToBuffer();
    firstOptimBuffer_ += firstOptimSimulationTable_.buffer();
    firstOptimSimulationTable_.clear();

    secondOptimSimulationTable_.writeToBuffer();
    secondOptimBuffer_ += secondOptimSimulationTable_.buffer();
    secondOptimSimulationTable_.clear();
}

SimulationTable* OptimisationsSimulationTable::firstOptimSimulationTable()
{
    return &firstOptimSimulationTable_;
}

SimulationTable* OptimisationsSimulationTable::secondOptimSimulationTable()
{
    return &secondOptimSimulationTable_;
}

std::string OptimisationsSimulationTable::headerCsvFormat() const
{
    return firstOptimSimulationTable_.headerCsvFormat();
}
