#pragma once

#include "ortools/linear_solver/linear_solver.h"
extern "C"
{
# include "ortools_wrapper.h"
}

extern bool withOrtool;

size_t current_memory_usage(std::string const & message);

//TODO JMK : to be updated when develop is merged with feature/cmake_build_dependency
std::list<std::string> GetOrtoolsSolverNames();

std::string getRunName(std::string const & prefix, size_t numSpace, int numInterval, int numOptim);

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(operations_research::MPSolver * solver, size_t numSpace, int const n);


namespace Antares {

    namespace Data {

        using namespace operations_research;

        /*! Enum class to define ortools solvers used */
        enum class OrtoolsSolver : unsigned char {
            sirius,     /*! Sirius solver  */
            coin,       /*! Cpl and Cbc coin solver */
            xpress,     /*! Xpress solver : licence needed, depends on ortools link */
            glop_scip,  /*! Glop and SCIP coin solver */
            cplex,      /*! CPlex solver */
            gurobi,     /*! Gurobi solver */
            glpk        /*! GLPK solver */
        };

        //TODO JMK : where should we store this information
        //Ortool solver used for simulation
        extern OrtoolsSolver OrtoolsEnumUsed;

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
        *  \brief Is ortools solver available with current linked ortools
        *
        *  \param ortoolsSolver : OrtoolsSolver enum
        *  \return true if ortools solver is  available
        */
        bool isOrtoolsSolverAvailable(OrtoolsSolver ortoolsSolver);

        /*!
        *  \brief Get ortools optimization probleme type for simplex from OrtoolsSolver enum
        *
        *  \param ortoolsSolver : OrtoolsSolver enum
        *  \return MPSolver::OptimizationProblemType
        */
        MPSolver::OptimizationProblemType getSimplexOptimProblemType(const OrtoolsSolver& ortoolsSolver);

        //TODO JMK : returned values must be adequacy with kOptimizationProblemTypeNames for ortools linear_solver/linear_solver.cc file
        //This is a temporary workaround before a ortools branch update
        std::string getSimplexOrtoolsSolverName(const OrtoolsSolver& ortoolsSolver);

        /*!
        *  \brief Get ortools optimization probleme type for PNE from OrtoolsSolver enum
        *
        *  \param ortoolsSolver : OrtoolsSolver enum
        *  \return MPSolver::OptimizationProblemType
        */
        MPSolver::OptimizationProblemType getPNEOptimProblemType(const OrtoolsSolver& ortoolsSolver);

        //TODO JMK : returned values must be adequacy with kOptimizationProblemTypeNames for ortools linear_solver/linear_solver.cc file
        //This is a temporary workaround before a ortools branch update
        std::string getPNEOrtoolsSolverName(const OrtoolsSolver& ortoolsSolver);



    }  // namespace Data

}  // namespace Antares