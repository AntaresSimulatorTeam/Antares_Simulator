#pragma once

#include <map>

#include <i_writer.h>

#include "ortools/linear_solver/linear_solver.h"

#include "ortools_wrapper.h"

using namespace operations_research;

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver,
                                                   uint year,
                                                   uint week,
                                                   uint const numOptim,
                                                   Antares::Solver::IResultWriter::Ptr writer);

/*!
 *  \brief Return list of available ortools solver name on our side
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();


/*!
 *  \brief Create a MPSolver with correct linear or mixed variant
 *
 *  \return MPSolver
 */
MPSolver* MPSolverFactory(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME *probleme, const std::string & solverName);

std::string generateTempPath(const std::string& filename);
void removeTemporaryFile(const std::string& tmpPath);


class OrtoolsUtils
{
public:
    struct SolverNames
    {
        std::string LPSolverName, MIPSolverName;
    };
    static const std::map<std::string, struct SolverNames> solverMap;

};

namespace Antares
{
namespace Optimization
{
MPSolver* convert_to_MPSolver(
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
}
} // namespace Antares
