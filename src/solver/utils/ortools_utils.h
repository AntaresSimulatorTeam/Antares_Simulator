#pragma once

#include <map>

#include <i_writer.h>

#include "ortools/linear_solver/linear_solver.h"

#include "ortools_wrapper.h"

using namespace operations_research;

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver, size_t numSpace, int const n, Antares::Solver::IResultWriter::Ptr writer);

/*!
 *  \brief Return list of available ortools solver name on our side
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();

/*!
 *  \brief Return list of available ortools solver name, linear and mixed
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverNameLinearAndMixed();

MPSolver* factory(Antares::Optimization::PROBLEME_SIMPLEXE_NOMME *probleme, std::string solverName);


class OrtoolsUtils
{
public:
    static const std::map<std::string, std::pair<std::string, std::string>> solverMap;

};

namespace Antares
{
namespace Optimization
{
MPSolver* convert_to_MPSolver(
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
}
} // namespace Antares
