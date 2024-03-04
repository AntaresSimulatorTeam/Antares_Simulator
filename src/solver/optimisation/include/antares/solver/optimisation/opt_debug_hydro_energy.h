#pragma once

#include <antares/writer/i_writer.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <vector>

class DebugHydroEnergy
{
public:
    DebugHydroEnergy(Antares::Solver::IResultWriter& writer,
                     PROBLEME_HEBDO* problemeHebdo);
    void write(int areaIndex, unsigned interval, const std::vector<double>& CntEnergieH2OParIntervalleOptimise);
private:
    Antares::Solver::IResultWriter& writer;
    PROBLEME_HEBDO* problemeHebdo;
};
