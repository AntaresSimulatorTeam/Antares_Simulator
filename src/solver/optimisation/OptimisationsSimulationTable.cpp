#include "include/antares/solver/optimisation/OptimisationsSimulationTable.h"

#include "antares/writer/i_writer.h"

void OptimisationsSimulationTable::clear()
{
    firstOptimBuffer_.clear();
    secondOptimBuffer_.clear();
}

std::pair<std::string, std::string> OptimisationsSimulationTable::buffers() const
{
    return {firstOptimBuffer_, secondOptimBuffer_};
}

void OptimisationsSimulationTable::write()
{
    firstOptimSimulationTable_.write();
    firstOptimBuffer_ += firstOptimSimulationTable_.buffer();
    firstOptimSimulationTable_.clear();

    secondOptimSimulationTable_.write();
    secondOptimBuffer_ += secondOptimSimulationTable_.buffer();
    secondOptimSimulationTable_.clear();
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

std::string OptimisationsSimulationTable::getHeader() const
{
    return firstOptimSimulationTable_.getHeader();
}
