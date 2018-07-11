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

   FONCTION: Pour les variables dont une seule ou aucune borne n'est connue
	           on cherche des contraintes qui permettent d'etablir la borne
						 manquante.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES 0
# define MAX_ITER 10

/*----------------------------------------------------------------------------*/

void PRS_BornerLesVariablesNonBornees( PRESOLVE * Presolve, int * NombreDeBornesCalculees )
{
int Cnt; int * Cdeb; int * Csui; int NombreDeVariables; char * ContrainteInactive;
int * NumContrainte; int ic; int Var; double * A;  int * TypeDeBornePourPresolve;
int Nb; int NbIter; PROBLEME_PNE * Pne;

*NombreDeBornesCalculees = 0;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeVariables = Pne->NombreDeVariablesTrav;  

A = Pne->ATrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ContrainteInactive = Presolve->ContrainteInactive;

NbIter = 0;
while ( NbIter < MAX_ITER ) {
  NbIter++;
  Nb = *NombreDeBornesCalculees;
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;
    if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) continue;
    /* On cherche une contrainte qui pourrait permettre de borner la variable */
	  ic = Cdeb[Var];
    while ( ic >= 0 ) {
		  if ( A[ic] != 0 ) {
	      Cnt = NumContrainte[ic];
        if ( ContrainteInactive[Cnt] != OUI_PNE ) {
          PRS_TenterUnCalculDeBorne( Presolve, Cnt, Var, NombreDeBornesCalculees );
				  /* Le type de borne a pu changer */
          if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) break;
          if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) break;								
		    }
		  }
		  ic = Csui[ic];
    }   
  }
	if ( *NombreDeBornesCalculees == Nb ) break;
}

#if VERBOSE_PRS
  printf("-> Nombre de bornes infinies qui ont pu etre ramenees a une valeur finie: %d\n",*NombreDeBornesCalculees);	
#endif

return;
}

/*----------------------------------------------------------------------------*/

void PRS_TenterUnCalculDeBorne( PRESOLVE * Presolve, int Cnt, int VarTest, int * NombreDeBornesCalculees )
{ 
char BorneInfCalculee; char BorneSupCalculee; double BorneInf; double BorneSup;
char UneBorneAEteCalculee;

UneBorneAEteCalculee = NON_PNE;

PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( Presolve, Cnt, VarTest, &BorneInfCalculee, &BorneInf,
																					           &BorneSupCalculee, &BorneSup );
																										 
if ( BorneInfCalculee == NON_PNE && BorneSupCalculee == NON_PNE ) return;

/* Remarque: on est forcement dans le cas ou il manque 1 ou 2 bornes */

PRS_MettreAJourLesBornesDUneVariable( Presolve, VarTest,
																		  BorneInfCalculee, BorneInf, Cnt,
																		  BorneSupCalculee, BorneSup, Cnt,
                                      &UneBorneAEteCalculee );
																			
if ( UneBorneAEteCalculee == OUI_PNE ) *NombreDeBornesCalculees = *NombreDeBornesCalculees + 1;

return;
}
  
