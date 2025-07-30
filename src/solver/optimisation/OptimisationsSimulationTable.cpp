#include "include/antares/solver/optimisation/OptimisationsSimulationTable.h"

#include "antares/writer/i_writer.h"

void OptimisationsSimulationTable::clear()
{
    firstOptimSimulationTable_.clear();
    secondOptimSimulationTable_.clear();
}

std::pair<std::string, std::string> OptimisationsSimulationTable::buffers() const
{
    return {firstOptimSimulationTable_.buffer(), secondOptimSimulationTable_.buffer()};
}

void OptimisationsSimulationTable::write()
{
    firstOptimSimulationTable_.write();
    firstOptimBuffer_ += firstOptimSimulationTable_.buffer();
    secondOptimSimulationTable_.write();
    secondOptimBuffer_ += secondOptimSimulationTable_.buffer();
}

void OptimisationsSimulationTable::writeTo(const std::string& filePrefix,
                                           Antares::Solver::IResultWriter& writer)
{
    writer.addEntryFromBuffer(filePrefix + "--optim-nb-1.csv", firstOptimBuffer_);
    writer.addEntryFromBuffer(filePrefix + "--optim-nb-2.csv", secondOptimBuffer_);
}

ISimulationTable& OptimisationsSimulationTable::firstOptimSimulationTable()
{
    return firstOptimSimulationTable_;
}

ISimulationTable& OptimisationsSimulationTable::secondOptimSimulationTable()
{
    return secondOptimSimulationTable_;
}
