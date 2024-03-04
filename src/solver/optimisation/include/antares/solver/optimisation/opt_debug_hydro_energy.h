#include <antares/writer/i_writer.h>
#include <vector>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

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
