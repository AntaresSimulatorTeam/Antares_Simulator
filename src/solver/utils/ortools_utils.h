#pragma once

#include <map>

#include <antares/study/OrtoolsSolver.hpp>
#include <i_writer.h>

#include "ortools/linear_solver/linear_solver.h"

#include "ortools_wrapper.h"

using namespace operations_research;

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver, size_t numSpace, int const n, Antares::Solver::IResultWriter::Ptr writer);

/*!
 *  \brief Return list of available ortools solver name
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();

namespace Antares
{
namespace Optimization
{
MPSolver* convert_to_MPSolver(
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
}
} // namespace Antares
