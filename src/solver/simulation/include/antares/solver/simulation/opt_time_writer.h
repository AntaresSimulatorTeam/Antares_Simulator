// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <sstream>

#include <antares/writer/i_writer.h>
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "simulation.h"

class OptimizationStatisticsWriter final
{
public:
    void addTime(uint week, const TIME_MEASURES& timeMeasure);
    OptimizationStatisticsWriter(Antares::Solver::IResultWriter& writer, uint year);
    void finalize();

private:
    void printHeader();
    std::ostringstream pBuffer;
    uint pYear;
    Antares::Solver::IResultWriter& pWriter;
};
