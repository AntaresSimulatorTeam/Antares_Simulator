#pragma once

#include <map>

#include <antares/study/OrtoolsSolver.hpp>

#include "ortools/linear_solver/linear_solver.h"
extern "C"
{
#include "ortools_wrapper.h"
}

using namespace operations_research;

std::string getRunName(std::string const& prefix, size_t numSpace, int numInterval, int numOptim);

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver, size_t numSpace, int const n);

using namespace Antares::Data;

class OrtoolsUtils
{
public:
    OrtoolsUtils();

    /*!
     *  \brief Is ortools solver available with current linked ortools
     *
     *  \param ortoolsSolver : OrtoolsSolver enum
     *  \return true if ortools solver is  available
     */
    bool isOrtoolsSolverAvailable(OrtoolsSolver ortoolsSolver);

    /*!
     *  \brief Return list of available ortools solver
     *
     *  \return List of available ortools solver
     */
    std::list<OrtoolsSolver> getAvailableOrtoolsSolver();

    /*!
     *  \brief Return list of available ortools solver name
     *
     *  \return List of available ortools solver name
     */
    std::list<std::string> getAvailableOrtoolsSolverName();

    /*!
     *  \brief Get ortools optimization probleme type for linear problem from OrtoolsSolver enum
     *
     *  \param ortoolsSolver : OrtoolsSolver enum
     *  \return MPSolver::OptimizationProblemType
     */
    MPSolver::OptimizationProblemType getLinearOptimProblemType(const OrtoolsSolver& ortoolsSolver);

    /*!
     *  \brief Get ortools optimization probleme type for mixed integer problem from OrtoolsSolver
     * enum
     *
     *  \param ortoolsSolver : OrtoolsSolver enum
     *  \return MPSolver::OptimizationProblemType
     */
    MPSolver::OptimizationProblemType getMixedIntegerOptimProblemType(
      const OrtoolsSolver& ortoolsSolver);

private:
    // TODO JMK : for now we need to use a string because we can't get all optimization problem type
    // with current ortools RTE branch :
    // can't use enum because of compile switch
    std::map<OrtoolsSolver, std::string> _solverLinearProblemOptimStringMap;
    std::map<OrtoolsSolver, std::string> _solverMixedIntegerProblemOptimStringMap;

    /*
    std::map<OrtoolsSolver, MPSolver::OptimizationProblemType> _solverLinearProblemOptimMap;
    std::map<OrtoolsSolver, MPSolver::OptimizationProblemType> _solverMixedIntegerProblemOptimMap;
        */
};

// C API
MPSolver* convert_to_MPSolver(const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);

