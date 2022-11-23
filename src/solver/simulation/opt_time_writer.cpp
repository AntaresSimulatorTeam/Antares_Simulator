#include "opt_time_writer.h"

OptimizationStatisticsWriter::OptimizationStatisticsWriter(
  Antares::Solver::IResultWriter::Ptr writer,
  uint year) :
 pWriter(writer)
{
    printHeader();
#define SEP Yuni::IO::Separator
    pFilename << "debug" << SEP << "solver" << SEP << "weeksSolveTimes_" << year << ".txt";
#undef SEP
}

void OptimizationStatisticsWriter::printHeader()
{
    pBuffer << "# Week Optimization_1_ms Optimization_2_ms\n";
}

void OptimizationStatisticsWriter::addTime(uint week, double opt_1_ms, double opt_2_ms)
{
    pBuffer << week << " " << opt_1_ms << " " << opt_2_ms << "\n";
}

void OptimizationStatisticsWriter::finalize()
{
    pWriter->addEntryFromBuffer(pFilename.c_str(), pBuffer);
}
