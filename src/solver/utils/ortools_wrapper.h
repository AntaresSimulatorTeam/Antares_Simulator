#ifndef __ORTOOLS_WRAPPER__
#define __ORTOOLS_WRAPPER__

#include <string>
#include "named_problem.h"
#include "ortools_utils.h"

using namespace operations_research;

MPSolver* ORTOOLS_Simplexe(Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                           MPSolver* ProbSpx,
                           bool keepBasis);

MPSolver* ORTOOLS_ConvertIfNeeded(const std::string& solverName,
                                  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                                  const Antares::Optimization::IMixedIntegerProblemManager& mixedProblemManager,
                                  MPSolver* solver);

void ORTOOLS_ModifierLeVecteurCouts(MPSolver* ProbSpx, const double* costs, int nbVar);
void ORTOOLS_ModifierLeVecteurSecondMembre(MPSolver* ProbSpx,
                                           const double* rhs,
                                           const char* sens,
                                           int nbRow);
void ORTOOLS_CorrigerLesBornes(MPSolver* ProbSpx,
                               const double* bMin,
                               const double* bMax,
                               const int* typeVar,
                               int nbVar);
void ORTOOLS_LibererProbleme(MPSolver* ProbSpx);

#endif
