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

   FONCTION: Effectue le changement de base apres avoir choisi les 
             variables a echanger.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_define.h"
# include "spx_fonctions.h"

# include "lu_define.h"

/*----------------------------------------------------------------------------*/

void SPX_FaireLeChangementDeBase( PROBLEME_SPX * Spx )
{
int Cnt; int SuccesUpdate; int VariableEntrante; int VariableSortante;
int i; int * BoundFlip; char * PositionDeLaVariable; int Var;
char * StatutBorneSupCourante; char * TypeDeVariable; double * Xmax;

/* On effectue les bound flip eventuels */
BoundFlip            = Spx->BoundFlip;   
PositionDeLaVariable = Spx->PositionDeLaVariable;
StatutBorneSupCourante = Spx->StatutBorneSupCourante;
TypeDeVariable = Spx->TypeDeVariable;
Xmax = Spx->Xmax;

for ( i = 0 ; i < Spx->NbBoundFlip ; i++ ) {
  if ( BoundFlip[i] > 0 ) { 
	  /*
		Var = BoundFlip[i]-1;
		printf("Bound flip vers BORNE_SUP variable %d\n", Var);
		*/
	  PositionDeLaVariable[BoundFlip[i]-1] = HORS_BASE_SUR_BORNE_SUP;		
	}
	else {	  
		Var = -BoundFlip[i]-1;
		/*
		printf("Bound flip vers BORNE_INF variable %d\n", Var);
		*/
	  PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
    # ifdef UTILISER_BORNES_AUXILIAIRES
      /* Si c'est une variable avec des bornes auxilaires il faut remettre la borne native */
		  if ( Spx->NombreDeBornesAuxiliairesUtilisees > 0 ) {	
        if ( StatutBorneSupCourante[Var] == BORNE_AUXILIAIRE_DE_VARIABLE_BORNEE_INFERIEUREMENT ) {
				  SPX_SupprimerUneBorneAuxiliaire( Spx, Var );
   	    }
			}
    # endif  
	}
}
  
VariableEntrante = Spx->VariableEntrante;
VariableSortante = Spx->VariableSortante;

# ifdef UTILISER_BORNES_AUXILIAIRES
  /* Si la variable entrante avait sur une borne sup auxiliaire on libere la borne */
	/* A verifier mais il me semble que c'est inutile car c'est fait dans SPX_MajDesVariablesEnBaseAControler
	   qui est appele avant */
  if ( Spx->NombreDeBornesAuxiliairesUtilisees > 0 ) SPX_SupprimerUneBorneAuxiliaire( Spx, VariableEntrante );
# endif  

SPX_MajDuTableauDesVariablesHorsBase( Spx );

Debut:

if ( Spx->FactoriserLaBase == OUI_SPX ) {

  Cnt = Spx->ContrainteDeLaVariableEnBase[VariableSortante];
  Spx->ContrainteDeLaVariableEnBase[VariableSortante] = -1;
  Spx->ContrainteDeLaVariableEnBase[VariableEntrante] = Cnt;
  Spx->VariableEnBaseDeLaContrainte[Cnt] = VariableEntrante;

  PositionDeLaVariable[VariableEntrante] = EN_BASE_LIBRE;  
                                                                     
  if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
    PositionDeLaVariable[VariableSortante] = HORS_BASE_SUR_BORNE_INF;
  }
  else {
    PositionDeLaVariable[VariableSortante] = HORS_BASE_SUR_BORNE_SUP;
  }
  
  /* Refactorisation de la nouvelle base */
  SPX_FactoriserLaBase( Spx );
  
}
else {
  SPX_MettreAJourLaBase( Spx , &SuccesUpdate );
  if ( Spx->UtiliserLaLuUpdate == OUI_SPX ) {
    /* En cas d'echec de la mise a jour on tente une factorisation complete */
    if ( SuccesUpdate == NON_LU ) {    
      Spx->FactoriserLaBase = OUI_SPX;
      if ( Spx->StrongBranchingEnCours != OUI_SPX ) goto Debut;
      return; /* On passe jamais par la */
    }
  }
	
  Cnt = Spx->ContrainteDeLaVariableEnBase[VariableSortante];

	if ( Spx->UtiliserLaLuUpdate == OUI_SPX ) {
	
	  /* Pour pouvoir faire du raffinement iteratif */
		/* Sinon il faut transmettre la matrice qui correspond a la matrice factorisee */
    if ( Spx->UtiliserLaBaseReduite == OUI_SPX ) {
      /* Attention il ne faut pas prendre Cnt mais l'ordre */
			i = Spx->OrdreColonneDeLaBaseFactorisee[Cnt];		
      Spx->CdebBase [i] = Spx->CdebProblemeReduit[VariableEntrante];
      Spx->NbTermesDesColonnesDeLaBase[i] = Spx->CNbTermProblemeReduit[VariableEntrante];		
		}
		else {		
      Spx->CdebBase [Cnt] = Spx->Cdeb[VariableEntrante];
      Spx->NbTermesDesColonnesDeLaBase[Cnt] = Spx->CNbTerm[VariableEntrante];
		}

	}

  Spx->ContrainteDeLaVariableEnBase[VariableSortante] = -1;
  Spx->ContrainteDeLaVariableEnBase[VariableEntrante] = Cnt;
  Spx->VariableEnBaseDeLaContrainte[Cnt] = VariableEntrante;

  PositionDeLaVariable[VariableEntrante] = EN_BASE_LIBRE;
		
  if ( Spx->SortSurXmaxOuSurXmin == SORT_SUR_XMIN ) {
    PositionDeLaVariable[VariableSortante] = HORS_BASE_SUR_BORNE_INF;
  }
  else {
    PositionDeLaVariable[VariableSortante] = HORS_BASE_SUR_BORNE_SUP;
  }

}

return;
}

