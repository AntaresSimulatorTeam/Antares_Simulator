#include "include/antares/solver/optimisation/OptimisationsSimulationTable.h"

#include "antares/writer/i_writer.h"
using namespace Antares::IO::Outputs;

void OptimisationsSimulationTable::clear()
{
    firstOptimBuffer_.clear();
    secondOptimBuffer_.clear();
}

std::pair<std::string, std::string> OptimisationsSimulationTable::moveBuffers()
{
    std::pair<std::string, std::string> result{std::move(firstOptimBuffer_),
                                               std::move(secondOptimBuffer_)};
    clear(); // gp : if we move, buffers get cleared, so no need to clear again.
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

void OptimisationsSimulationTable::writeTo(const std::string& filePrefix,
                                           Antares::Solver::IResultWriter& writer)
{
    writer.addEntryFromBuffer(filePrefix + "--optim-nb-1.csv", firstOptimBuffer_);
    writer.addEntryFromBuffer(filePrefix + "--optim-nb-2.csv", secondOptimBuffer_);
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
