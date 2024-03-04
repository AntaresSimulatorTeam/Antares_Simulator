#include "antares/solver/optimisation/opt_debug_hydro_energy.h"
#include <yuni/io/file.h>

DebugHydroEnergy::DebugHydroEnergy(bool doWrite,
				   Antares::Solver::IResultWriter& writer,
                                   const PROBLEME_HEBDO* problemeHebdo) : doWrite(doWrite), writer(writer), problemeHebdo(problemeHebdo) {}

void DebugHydroEnergy::write(int areaIndex, unsigned interval, const std::vector<double>& CntEnergieH2OParIntervalleOptimise) const
{
  if (!doWrite)
      return;

  std::ostringstream buffer, path;
  const unsigned y = problemeHebdo->year;
#define SEP Yuni::IO::Separator
  path
    << "debug" << SEP
    << "solver" << SEP
    << (1 + y) << SEP
    << "week-" << problemeHebdo->weekInTheYear
    << "interval-" << interval
    << "_" << problemeHebdo->NomsDesPays[areaIndex] << ".txt";
#undef SEP
  for (double v : CntEnergieH2OParIntervalleOptimise)
  {
      buffer << v << '\n';
  }
  std::string str = buffer.str();
  writer.addEntryFromBuffer(path.str(), str);
}
