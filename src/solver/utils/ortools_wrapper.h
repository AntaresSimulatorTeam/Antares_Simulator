#ifndef __ORTOOLS_WRAPPER__
#define __ORTOOLS_WRAPPER__

#include "pne_definition_arguments.h"
#include "pne_fonctions.h"

#include "named_problem.h"

using namespace operations_research;

MPSolver* ORTOOLS_Simplexe(PROBLEME_SIMPLEXE_NOMME* Probleme,
                           MPSolver* ProbSpx);
MPSolver* ORTOOLS_Simplexe_PNE(PROBLEME_A_RESOUDRE* Probleme, MPSolver* ProbSpx);
void ORTOOLS_ModifierLeVecteurCouts(MPSolver* ProbSpx, double* costs, int nbVar);
void ORTOOLS_ModifierLeVecteurSecondMembre(MPSolver* ProbSpx, double* rhs, char* sens, int nbRow);
void ORTOOLS_CorrigerLesBornes(MPSolver* ProbSpx,
                               double* bMin,
                               double* bMax,
                               int* typeVar,
                               int nbVar);
void ORTOOLS_LibererProbleme(MPSolver* ProbSpx);

#endif
