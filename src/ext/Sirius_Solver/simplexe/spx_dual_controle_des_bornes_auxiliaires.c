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

   FONCTION: Controle des bornes auxiliaires apres convergence.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "bb_define.h"		        
# include "bb_fonctions.h"

# include "pne_define.h"

# define COEFF_MULTIPLICATEUR 2.0

/*----------------------------------------------------------------------------*/

# ifdef UTILISER_BORNES_AUXILIAIRES

/*----------------------------------------------------------------------------*/

void SPX_DualRepositionnerLesBornes( PROBLEME_SPX * Spx, char * Changements )
{
int Var; char ChangementDeBornes; char * StatutBorneSupCourante; char InfaisabiliteDuale;
char * PositionDeLaVariable; char * TypeDeVariable; double * Xmax; double * Xmin;

InfaisabiliteDuale = NON_SPX;  
ChangementDeBornes = NON_SPX;
StatutBorneSupCourante = Spx->StatutBorneSupCourante;
TypeDeVariable         = Spx->TypeDeVariable;
Xmax                   = Spx->Xmax;
Xmin                   = Spx->Xmin;
PositionDeLaVariable   = Spx->PositionDeLaVariable;	
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
	if ( StatutBorneSupCourante[Var] == BORNE_NATIVE ) continue;			
	Xmax[Var] *= COEFF_MULTIPLICATEUR; 
  #if VERBOSE_SPX 	
	  printf("Augmentation de borne pour la variable %d, nouvelle borne %e\n",Var,Xmax[Var]);
	# endif	
  if ( Xmax[Var] < LINFINI_SPX ) ChangementDeBornes = OUI_SPX;
	else {
	  /* Si Xmax devient trop grand on sort avec le diagnostique existant */  
		/*
		printf("!!! Spx_DualRepositionnerLesBornes: probleme, on n'arrive pas a avoir une solution duale admissible meme\n");
		printf("en faisant tendre les bornes auxiliaires vers l'infini  iteration %d\n",Spx->Iteration);
		if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) printf("Variable HORS_BASE_SUR_BORNE_INF  ");
		if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) printf("Variable HORS_BASE_SUR_BORNE_SUP  ");
		if ( PositionDeLaVariable[Var] == HORS_BASE_A_ZERO ) printf("Variable HORS_BASE_A_ZERO  ");
		if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) printf("EN_BASE_LIBRE  ");
		if ( Spx->StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_BORNEE_INFERIEUREMENT ) printf("BORNE_AUXILIAIRE_DE_VARIABLE_BORNEE_INFERIEUREMENT\n");
		if ( Spx->StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) printf("BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE\n");		
		*/
		if ( PositionDeLaVariable[Var] == EN_BASE_LIBRE ) {
		  printf("BUG dans SPX_DualRepositionnerLesBornes, une variable en base ne peut pas avoir de borne auxiliaire\n");
			exit(0);
		}
		/* On enleve la borne auxiliaire car sinon ca peut faire planter dans un branch and bound puisqu'on ne reinitialise
		   pas le statut des bornes sup courantes */
    SPX_SupprimerUneBorneAuxiliaire( Spx, Var );
    InfaisabiliteDuale = OUI_SPX;			 
	}	
}

if ( InfaisabiliteDuale == OUI_SPX ) {
	/* Il faut remettre toute les bornes natives car en cas de branch and bound cela peut faire
	   faire planter les simplexes suivants */
  for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
	  if ( StatutBorneSupCourante[Var] == BORNE_NATIVE ) continue;		
		/* On enleve la borne auxiliaire car sinon ca peut faire planter dans un branch and bound puisqu'on ne reinitialise
		   pas le statut des bornes sup courantes */
    SPX_SupprimerUneBorneAuxiliaire( Spx, Var );		
  }
  /* Afin de ne pas relancer une phase de simplexe */
  ChangementDeBornes = NON_SPX;
  Spx->YaUneSolution = NON_SPX;	
}

*Changements = ChangementDeBornes;

return;
}

/*----------------------------------------------------------------------------*/

void SPX_DualControlerLesBornesAuxiliaires( PROBLEME_SPX * Spx, char * Reoptimiser )

{
char ChangementDeBornes;

*Reoptimiser = NON_SPX;
if ( Spx->NombreDeBornesAuxiliairesUtilisees == 0 ) return;

/* Qu'il y ait une solution ou non, il faut remettre les les bornes dans leur etat initial
   puis relancer une phase 1 */
/* printf("NombreDeBornesAuxiliairesUtilisees: %d   YaUneSolution %d\n", Spx->NombreDeBornesAuxiliairesUtilisees,(int) Spx->YaUneSolution); */

Spx->CoeffMultiplicateurDesBornesAuxiliaires *= COEFF_MULTIPLICATEUR;
SPX_DualRepositionnerLesBornes( Spx, &ChangementDeBornes );
if ( ChangementDeBornes == OUI_SPX ) {
  *Reoptimiser = OUI_SPX;
}

return;

SPX_DualRepositionnerLesBornes( Spx, &ChangementDeBornes );

Spx->NombreDeBornesAuxiliairesUtilisees = 0;

if ( ChangementDeBornes == OUI_SPX ) {
  /* On Refait une admissiblite duale et on relance la phase 2 */
	Spx->IterationPourBornesAuxiliaires *= 2;
	Spx->CoeffMultiplicateurDesBornesAuxiliaires *= COEFF_MULTIPLICATEUR;
  printf("On refait une admissibilite duale. IterationPourBornesAuxiliaires: %d\n",Spx->IterationPourBornesAuxiliaires);
		
  SPX_DualPhase1Simplexe( Spx );
  printf("NombreDeBornesAuxiliairesUtilisees apres la nouvelle phase 1: %d\n",Spx->NombreDeBornesAuxiliairesUtilisees);
	Spx->PhaseEnCours = PHASE_2;
  if ( Spx->LaBaseEstDualeAdmissible == NON_SPX ) {
    /* Echec */
		printf("On n'a pas pu trouver de base duale admissible \n");
    Spx->YaUneSolution = NON_SPX;
  }
	else {
	  *Reoptimiser = OUI_SPX;
	}
}

return;
}

/*----------------------------------------------------------------------------*/

# endif

/*----------------------------------------------------------------------------*/
