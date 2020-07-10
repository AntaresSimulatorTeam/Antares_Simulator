# ifndef __ORTOOLS_WRAPPER__
# define __ORTOOLS_WRAPPER__

# include "spx_definition_arguments.h"
# include "spx_fonctions.h"

# include "pne_definition_arguments.h"
# include "pne_fonctions.h"

# include "../optimisation/opt_structure_probleme_a_resoudre.h"

extern int withOrtool_c;

void * ORTOOLS_Simplexe(PROBLEME_SIMPLEXE * Probleme, void * ProbSpx);
void * ORTOOLS_Simplexe_PNE(PROBLEME_A_RESOUDRE * Probleme, void * ProbSpx);
void ORTOOLS_ModifierLeVecteurCouts(void * ProbSpx, double * costs, int nbVar);
void ORTOOLS_ModifierLeVecteurSecondMembre(void * ProbSpx, double * rhs, char * sens, int nbRow);
void ORTOOLS_CorrigerLesBornes(void * ProbSpx, double * bMin, double * bMax, int * typeVar, int nbVar, PROBLEME_SIMPLEXE * Probleme);
void ORTOOLS_LibererProbleme(void * ProbSpx);

# endif