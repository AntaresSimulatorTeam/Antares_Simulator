// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/opt_time_writer.h"

#include <filesystem>

OptimizationStatisticsWriter::OptimizationStatisticsWriter(Antares::Solver::IResultWriter& writer,
                                                           uint year):
    pYear(year),
    pWriter(writer)
{
    printHeader();
}

void OptimizationStatisticsWriter::printHeader()
{
    pBuffer << "# Week Optimization_1_ms Optimization_2_ms Update_ms1 Update_ms2\n";
}

void OptimizationStatisticsWriter::addTime(uint week, const TIME_MEASURES& timeMeasure)
{
    pBuffer << week << " " << timeMeasure[0].solveTime << " " << timeMeasure[1].solveTime << " "
            << timeMeasure[0].updateTime << " " << timeMeasure[1].updateTime << "\n";
}

void OptimizationStatisticsWriter::finalize()
{
    using path = std::filesystem::path;
    const path filename = path("optimization") / "week-by-week"
                          / ("year_" + std::to_string(pYear) + ".txt");
    std::string s = pBuffer.str();
    pWriter.addEntryFromBuffer(filename, s);
}
