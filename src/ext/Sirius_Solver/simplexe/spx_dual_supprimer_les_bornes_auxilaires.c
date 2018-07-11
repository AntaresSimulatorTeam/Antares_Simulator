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

   FONCTION: On essai de remettre les bornes initiales tout en restant
	           dual realisable 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# ifdef UTILISER_BORNES_AUXILIAIRES
/*----------------------------------------------------------------------------*/

void SPX_DualSupprimerLesBornesAuxiliaires( PROBLEME_SPX * Spx )
{
int Var; int i; int * NumerosDesVariablesHorsBase; double * CBarre; 
char * StatutBorneSupCourante; char * PositionDeLaVariable;
int NombreDeBornesAuxilairesAuDepart; double * SeuilDAmissibiliteDuale;

NumerosDesVariablesHorsBase = Spx->NumerosDesVariablesHorsBase;
CBarre                      = Spx->CBarre;
StatutBorneSupCourante      = Spx->StatutBorneSupCourante;
PositionDeLaVariable        = Spx->PositionDeLaVariable;
  
NombreDeBornesAuxilairesAuDepart = Spx->NombreDeBornesAuxiliairesUtilisees;

SeuilDAmissibiliteDuale = Spx->SeuilDAmissibiliteDuale2;

/* Boucle sur les variables hors base */
for ( i = 0 ; i < Spx->NombreDeVariablesHorsBase ; i++ ) {
  Var = NumerosDesVariablesHorsBase[i];		
	if ( StatutBorneSupCourante[Var] == BORNE_NATIVE ) continue;	

  if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_BORNEE_INFERIEUREMENT ) {      		
    /* Si la variable est devenue duale admissible avec sa borne native on peut la passer sur borne inf
		   et remettre son type de borne initial */
		if ( CBarre[Var] > -SeuilDAmissibiliteDuale[Var] ) {
      SPX_SupprimerUneBorneAuxiliaire( Spx, Var );				
      PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;		
		}
  }				
	else if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_NON_BORNEE ) {
    if ( fabs( CBarre[Var] ) < SeuilDAmissibiliteDuale[Var] ) {
      SPX_SupprimerUneBorneAuxiliaire( Spx, Var );		
      PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
		}
	}	
}

if ( Spx->NombreDeBornesAuxiliairesUtilisees != NombreDeBornesAuxilairesAuDepart ) {
  Spx->CalculerBBarre = OUI_SPX;
	# if VERBOSE_SPX
	  printf("Iteration %d Nombre de bornes auxiliaires utilisees %d\n",Spx->Iteration,Spx->NombreDeBornesAuxiliairesUtilisees);
	# endif
}

return;
}

# endif

