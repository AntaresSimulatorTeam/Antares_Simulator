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
