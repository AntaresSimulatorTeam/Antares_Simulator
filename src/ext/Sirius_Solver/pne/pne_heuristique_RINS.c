/*
** Copyright 2007-2018 RTE
** Author: Robert Gonzalez
**
** This file is part of Sirius_Solver.
** This program and the accompanying materials are made available under the
** terms of the Eclipse Public License 2.0 which is available at
** http://www.eclipse.org/legal/epl-2.0.
**
** This Source Code may also be made available under the following Secondary
** Licenses when the conditions for such availability set forth in the Eclipse
** Public License, v. 2.0 are satisfied: GNU General Public License, version 3
** or later, which is available at <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: EPL-2.0 OR GPL-3.0
*/
/***********************************************************************

   FONCTION: Methode RINS Danna - Rothberg - Le Pape 
	 
						 Attention, lorsque ce module est appele les variables
						 UminTrav UmaxTrav et TypeDeBorneTrav on ete remises a leur
						 valeur du noeud racine. Donc leurs valeurs n'inclut pas
						 les instanciations faites en amont du noeud a partir duquel
						 on demarre.
						 Par contre les structures du simplexe se trouvent dans l'etat
						 du dernier noeud resolu (bornes et types de bornes).						 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 1 

/*----------------------------------------------------------------------------*/
/* Methode RINS Danna - Rothberg - Le Pape */
char PNE_HeuristiqueRINS( PROBLEME_PNE * Pne ) 
{
int Var; double * UOpt; double * U; int NombreDeVariables; double Zero; int * TypeDeBorne;
double * Umin; double * Umax; int NbFix; BB * Bb; char OK; double * USv; int NbFixInit;
int NbLibres; MATRICE_DE_CONTRAINTES * Contraintes; char ProblemeTropGros;

ProblemeTropGros = NON_PNE;

Bb = (BB *) Pne->ProblemeBbDuSolveur;
Contraintes = NULL;

/* On fixe les variables qui ont la meme valeur dans la solution courante et dans la
   meilleure solution entiere */
Zero = 1.e-7;
NombreDeVariables = Pne->NombreDeVariablesTrav;
UOpt = Pne->UOpt;
U = Pne->UTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
NbFix = 0;  
NbFixInit = 0;
NbLibres = 0;

USv = Pne->ValeurLocale;
memcpy( (char *) USv, (char *) U, NombreDeVariables * sizeof( double ) );

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) NbFixInit++;
	else if ( Umin[Var] == Umax[Var] ) NbFixInit++;
	if ( Pne->TypeDeVariableTrav[Var] != ENTIER && 0 ) continue;
  if ( fabs( U[Var] - UOpt[Var] ) < Zero ) {				  
    TypeDeBorne[Var] = VARIABLE_FIXE;
		U[Var] = UOpt[Var];
		Umin[Var] = UOpt[Var];
		Umax[Var] = UOpt[Var];
		NbFix++;
 	}
}

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeBorne[Var] != VARIABLE_FIXE ) {
    if ( Umin[Var] != Umax[Var] ) NbLibres++;
	}
}

printf("Methode RINS  NbFix = %d  NbFixInit = %d  NbLibres = %d NombreDeVariables = %d\n",NbFix,NbFixInit,NbLibres,NombreDeVariables);

if ( NbFix <= 0 ) goto Termine;

OK = PNE_HeuristiqueEvaluerTailleDuProbleme( Pne );
if ( OK == NON_PNE ) ProblemeTropGros = OUI_PNE;

if ( OK == OUI_PNE ) {
  /* Construction de ma matrice des contraintes */
  Contraintes = PNE_HeuristiqueConstruireMatriceDeContraintes( Pne );
  if ( Contraintes == NULL ) goto Termine;

  /* Resolution du branch and bound reduit */
  OK = PNE_HeuristiqueResolutionBranchAndBoundReduit( Pne, Contraintes );
}

if ( OK == NON_PNE ) {	
  Pne->NombreDEchecsSuccessifsHeuristiqueRINS++;
	if ( Pne->NombreDEchecsSuccessifsHeuristiqueRINS >= NB_MAX_ECHECS_SUCCESSIFS_HEURISTIQUE && 0 ) {
    Pne->FaireHeuristiqueRINS = NON_PNE;
    if ( Pne->NombreDeSolutionsHeuristiqueRINS <= 0 ) Pne->NombreDeReactivationsSansSuccesHeuristiqueRINS++;
		if ( Pne->NombreDeReactivationsSansSuccesHeuristiqueRINS > NB_MAX_REACTIVATION_SANS_SUCCES ) {
      Pne->StopHeuristiqueRINS = OUI_PNE;
			printf("****************** arret definitif RINS \n");			
		}
		else printf(" !!!!!!!!!!!! On positionne a FaireHeuristiqueRINS NON_PNE \n");
	}
	goto Termine;	
}

/* On a trouve une solution et elle a ete controlee et archivee */

Pne->NombreDEchecsSuccessifsHeuristiqueRINS = 0;
Pne->NombreDeSolutionsHeuristiqueRINS++;
Pne->NombreDeReactivationsSansSuccesHeuristiqueRINS = 0;	
	
Termine:

Pne->CestTermine = NON_PNE;

memcpy( (char *) U, (char *) USv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) TypeDeBorne, (char *) Pne->TypeDeBorneTravSv, NombreDeVariables * sizeof( int ) );
memcpy( (char *) Umax, (char *) Pne->UmaxTravSv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Umin, (char *) Pne->UminTravSv, NombreDeVariables * sizeof( double ) );

PNE_HeuristiqueLibererMatriceDeContraintes( Contraintes );

return( ProblemeTropGros );
}


