#pragma once
#include <yuni/core/string.h>
#include <i_writer.h>

#include "simulation.h"

class OptimizationStatisticsWriter
{
public:
    OptimizationStatisticsWriter(Antares::Solver::IResultWriter::Ptr writer, uint year);
    void addTime(uint week, double opt_1_ms, double opt_2_ms);
    void finalize();

private:
    void printHeader();
    Yuni::Clob pBuffer;
    Yuni::String pFilename;
    Antares::Solver::IResultWriter::Ptr pWriter;
};
