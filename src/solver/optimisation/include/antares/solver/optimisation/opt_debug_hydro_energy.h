#pragma once

#include <antares/writer/i_writer.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <vector>

class DebugHydroEnergy
{
public:
    DebugHydroEnergy(bool doWrite,
		     Antares::Solver::IResultWriter& writer,
                     const PROBLEME_HEBDO* problemeHebdo);
    void write(int areaIndex, unsigned interval, const std::vector<double>& CntEnergieH2OParIntervalleOptimise) const;
private:
    const bool doWrite;
    Antares::Solver::IResultWriter& writer;
    const PROBLEME_HEBDO* problemeHebdo;
};
