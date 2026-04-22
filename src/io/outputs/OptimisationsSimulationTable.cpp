// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "include/antares/io/outputs/OptimisationsSimulationTable.h"
// #include "include/antares/writer/i_writer.h"

using namespace Antares::IO::Outputs;

std::pair<std::string, std::string> OptimisationsSimulationTable::moveBuffers()
{
    return {std::move(firstOptimBuffer_), std::move(secondOptimBuffer_)};
}

void OptimisationsSimulationTable::writeToBuffer()
{
    firstOptimSimulationTable_.writeToBuffer();
    firstOptimBuffer_ += firstOptimSimulationTable_.buffer();
    firstOptimSimulationTable_.clear();

    secondOptimSimulationTable_.writeToBuffer();
    secondOptimBuffer_ += secondOptimSimulationTable_.buffer();
    secondOptimSimulationTable_.clear();
}

//void OptimisationsSimulationTable::writeTo(const std::string& filePrefix,
//                                           Antares::Solver::IResultWriter& writer)
//{
//    writer.addEntryFromBuffer(filePrefix + "--optim-nb-1.csv", firstOptimBuffer_);
//    writer.addEntryFromBuffer(filePrefix + "--optim-nb-2.csv", secondOptimBuffer_);
//}

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
