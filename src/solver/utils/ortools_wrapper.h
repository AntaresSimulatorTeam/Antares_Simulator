#ifndef __ORTOOLS_WRAPPER__
#define __ORTOOLS_WRAPPER__

#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "pne_definition_arguments.h"
#include "pne_fonctions.h"

#include "../optimisation/opt_structure_probleme_a_resoudre.h"

using namespace operations_research;

MPSolver* ORTOOLS_Simplexe(PROBLEME_SIMPLEXE* Probleme, MPSolver* ProbSpx);
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