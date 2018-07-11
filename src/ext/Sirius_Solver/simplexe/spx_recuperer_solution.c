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

   FONCTION: Translater les bornes en sortie pour tout remettre     
             en ordre et restituer les resultats sur les variables
             initiales du probleme.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# define OLD_VERSION 1
# define NEW_VERSION 2
# define CHOIX_VERSION NEW_VERSION

/*----------------------------------------------------------------------------*/

void SPX_RecupererLaSolution( PROBLEME_SPX * Spx,
                              int     NbVar_E, 
                              double * X_E,
                              int   * TypeVar_E,  
                              int     NbContr_E,
                              int   * PositionDeLaVariable,	
                              int   * NbVarDeBaseComplementaires,
                              int   * ComplementDeLaBase
                            )
{
int Cnt; int Cnt_E; int Var_E; int VarSimplexe; char Position;
char * PositionDeLaVariableSPX;
# if CHOIX_VERSION == OLD_VERSION
  int * CorrespondanceVarEntreeVarSimplexe;
# endif
# if CHOIX_VERSION == NEW_VERSION
  int * CorrespondanceVarSimplexeVarEntree;
# endif
# if VERBOSE_PNE
  char * OrigineDeLaVariable;
# endif
int * Cdeb; int * NumeroDeContrainte; 
int * CorrespondanceCntSimplexeCntEntree; double * XminEntree;
char * TypeDeVariable; double * X; 

/*NbContr_E = 0;*/

SPX_UnScaling( Spx );

PositionDeLaVariableSPX = Spx->PositionDeLaVariable;

# if VERBOSE_PNE
  OrigineDeLaVariable = Spx->OrigineDeLaVariable;
# endif
CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;
# if CHOIX_VERSION == OLD_VERSION
  CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;    
# endif
# if CHOIX_VERSION == NEW_VERSION
  CorrespondanceVarSimplexeVarEntree = Spx->CorrespondanceVarSimplexeVarEntree;    
# endif

Cdeb = Spx->Cdeb;
NumeroDeContrainte = Spx->NumeroDeContrainte;

TypeDeVariable = Spx->TypeDeVariable;
X              = Spx->X;
XminEntree     = Spx->XminEntree;

/* Remettre la solution dans les variables d origine et renvoyer la base optimale */

# if CHOIX_VERSION == OLD_VERSION

for ( Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {
	PositionDeLaVariable[Var_E] = -1;
  /* Attention correspondance des contraintes */
  VarSimplexe = CorrespondanceVarEntreeVarSimplexe[Var_E];	
  if ( VarSimplexe >= 0 ) {
		Position = PositionDeLaVariableSPX[VarSimplexe];
    if ( Position == EN_BASE_LIBRE ) {
      /* Une variable native est basique. Si la contrainte de cette variable en base 
         est une coupe, on va considerer que cette coupe est saturee. En principe, elle 
         sera conservee. Cependant il peut s'agir d'un appel dans un contexte de 
         strong branching et dans ce cas elle risque d'etre supprimee si dans une des 
         instanciations a entier inf ou a entier sup la variable d'ecart associee a 
         la contrainte est basique */ 
      PositionDeLaVariable[Var_E] = EN_BASE;
		}		
    else if ( Position == HORS_BASE_SUR_BORNE_INF ) {   
      PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_INF;
    }
    else if ( Position == HORS_BASE_SUR_BORNE_SUP ) {
      PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_SUP;
    }
    else {
      PositionDeLaVariable[Var_E] = HORS_BASE_A_ZERO;
    }
  }
  else continue;
	
  if ( TypeVar_E[Var_E] != VARIABLE_FIXE ) {
    if ( TypeDeVariable[VarSimplexe] == BORNEE || TypeDeVariable[VarSimplexe] == BORNEE_INFERIEUREMENT ) {
      X_E[Var_E] = X[VarSimplexe] + XminEntree[VarSimplexe];			
    }
    else X_E[Var_E] = X[VarSimplexe];	
    if ( TypeVar_E[Var_E] == VARIABLE_BORNEE_SUPERIEUREMENT ) X_E[Var_E] = -X_E[Var_E];
  }
	
}

# elif CHOIX_VERSION == NEW_VERSION
	
for ( VarSimplexe = 0 ; VarSimplexe < Spx->NombreDeVariablesNatives ; VarSimplexe++ ) {
  /* Attention correspondance des contraintes */
  Var_E = CorrespondanceVarSimplexeVarEntree[VarSimplexe];	
  if ( Var_E < 0 || Var_E >= NbVar_E) continue;
	Position = PositionDeLaVariableSPX[VarSimplexe];
  if ( Position == EN_BASE_LIBRE ) {
    /* Une variable native est basique. Si la contrainte de cette variable en base 
       est une coupe, on va considerer que cette coupe est saturee. En principe, elle 
       sera conservee. Cependant il peut s'agir d'un appel dans un contexte de 
       strong branching et dans ce cas elle risque d'etre supprimee si dans une des 
       instanciations a entier inf ou a entier sup la variable d'ecart associee a 
       la contrainte est basique */ 
    PositionDeLaVariable[Var_E] = EN_BASE;
	}		
  else if ( Position == HORS_BASE_SUR_BORNE_INF ) {   
    PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_INF;
  }
  else if ( Position == HORS_BASE_SUR_BORNE_SUP ) {
    PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_SUP;
  }
  else {
    PositionDeLaVariable[Var_E] = HORS_BASE_A_ZERO;
  }
	
  if ( TypeVar_E[Var_E] != VARIABLE_FIXE ) {
    if ( TypeDeVariable[VarSimplexe] == BORNEE || TypeDeVariable[VarSimplexe] == BORNEE_INFERIEUREMENT ) {
      X_E[Var_E] = X[VarSimplexe] + XminEntree[VarSimplexe];			
    }
    else X_E[Var_E] = X[VarSimplexe];	
    if ( TypeVar_E[Var_E] == VARIABLE_BORNEE_SUPERIEUREMENT ) X_E[Var_E] = -X_E[Var_E];		
  }
	
}

# endif

/* Des contraintes natives peuvent ne pas avoir de variable de base si c'est une variables additionnelle.
   Il se peut alors que la variable additionnelle soit celle d'une autre contrainte. 
   On donne alors la matrice sous la forme d'un vecteur (puisqu'il s'agit de variables additionnelles) */
*NbVarDeBaseComplementaires = 0;
for ( VarSimplexe = Spx->NombreDeVariablesNatives ; VarSimplexe < Spx->NombreDeVariablesDuProblemeSansCoupes ; VarSimplexe++ ) {

  if ( PositionDeLaVariableSPX[VarSimplexe] != EN_BASE_LIBRE ) continue;
       
  #if VERBOSE_PNE
    if ( OrigineDeLaVariable[VarSimplexe] == BASIQUE_ARTIFICIELLE && Spx->StrongBranchingEnCours == NON_SPX ) {
      printf(" IL RESTE DES VARIABLES BASIQUES ARTIFICIELLES DANS LA BASE variable simplexe %d valeur %lf \n",
               VarSimplexe,Spx->X[VarSimplexe]);
    }
  #endif
  Cnt = NumeroDeContrainte[Cdeb[VarSimplexe]];		
  /* La variable est en base, la colonne de la variable doit avoir un seul terme */
  Cnt_E = CorrespondanceCntSimplexeCntEntree[Cnt];
	/* Attention ce n'est pas Spx->NombreDeContraintesDuProblemeSansCoupes mais le nombre de contraintes donnees au simplexe
	   lequel peut etre different de Spx->NombreDeContraintesDuProblemeSansCoupes si par exemple il y a des contraintes
		 vides */
  /*if ( Cnt_E >= 0 && Cnt_E < Spx->NombreDeContraintesDuProblemeSansCoupes ) {*/
  if ( Cnt_E >= 0 && Cnt_E < NbContr_E ) {
    ComplementDeLaBase[*NbVarDeBaseComplementaires] = Cnt_E;	
    *NbVarDeBaseComplementaires = *NbVarDeBaseComplementaires + 1;
  }
}

return;
}

/*---------------------------------------------------------------------------------*/

void SPX_RecuperationSimplifieeDeLaSolutionPourStrongBranching(
                              PROBLEME_SPX * Spx,
                              int     NbVar_E, 															
                              int   * PositionDeLaVariable,	
                              int   * NbVarDeBaseComplementaires,
                              int   * ComplementDeLaBase
                            )
{
int Cnt; int Cnt_E; int Var_E; int VarSimplexe; char Position;
char * PositionDeLaVariableSPX; int * CorrespondanceVarEntreeVarSimplexe;
int * Cdeb; int * NumeroDeContrainte; int * CorrespondanceCntSimplexeCntEntree;

CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
PositionDeLaVariableSPX            = Spx->PositionDeLaVariable;
CorrespondanceCntSimplexeCntEntree = Spx->CorrespondanceCntSimplexeCntEntree;
CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;    

Cdeb = Spx->Cdeb;
NumeroDeContrainte = Spx->NumeroDeContrainte;

for ( Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {
	PositionDeLaVariable[Var_E] = -1;
  /* Attention correspondance des contraintes */
  VarSimplexe = CorrespondanceVarEntreeVarSimplexe[Var_E];	
  if ( VarSimplexe >= 0 ) {
		Position = PositionDeLaVariableSPX[VarSimplexe];
    if ( Position == EN_BASE_LIBRE ) {
      /* Une variable native est basique. Si la contrainte de cette variable en base 
         est une coupe, on va considerer que cette coupe est saturee. En principe, elle 
         sera conservee. Cependant il peut s'agir d'un appel dans un contexte de 
         strong branching et dans ce cas elle risque d'etre supprimee si dans une des 
         instanciations a entier inf ou a entier sup la variable d'ecart associee a 
         la contrainte est basique */ 
      PositionDeLaVariable[Var_E] = EN_BASE;
		}		
    else if ( Position == HORS_BASE_SUR_BORNE_INF ) {   
      PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_INF;
    }
    else if ( Position == HORS_BASE_SUR_BORNE_SUP ) {
      PositionDeLaVariable[Var_E] = HORS_BASE_SUR_BORNE_SUP;
    }
    else {
      PositionDeLaVariable[Var_E] = HORS_BASE_A_ZERO;
    }
  }	
}

/* Des contraintes natives peuvent ne pas avoir de variable de base si c'est une variables additionnelle.
   Il se peut alors que la variable additionnelle soit celle d'une autre contrainte. 
   On donne alors la matrice sous la forme d'un vecteur (puisqu'il s'agit de variables additionnelles) */
*NbVarDeBaseComplementaires = 0;
for ( VarSimplexe = Spx->NombreDeVariablesNatives ; VarSimplexe < Spx->NombreDeVariablesDuProblemeSansCoupes ; VarSimplexe++ ) {

  if ( PositionDeLaVariableSPX[VarSimplexe] != EN_BASE_LIBRE ) continue;
       
  Cnt = NumeroDeContrainte[Cdeb[VarSimplexe]];		
  /* La variable est en base, la colonne de la variable doit avoir un seul terme */
  Cnt_E = CorrespondanceCntSimplexeCntEntree[Cnt];
  if ( Cnt_E >= 0 && Cnt_E < Spx->NombreDeContraintesDuProblemeSansCoupes ) {
    ComplementDeLaBase[*NbVarDeBaseComplementaires] = Cnt_E;	
    *NbVarDeBaseComplementaires = *NbVarDeBaseComplementaires + 1;
  }
}

return;
}

/*---------------------------------------------------------------------------------*/
/*  Recuperation de la solution sur les coupes. La seule information interessante 
    est "la coupe est saturee ou pas". Si la variable d'ecart est basique, la coupe 
    n'est pas saturee (ce qui n'est pas tout a fait vrai). Si la variable d'ecart 
    n'est pas basique, la coupe est saturee.                                       */

void SPX_RecupererLaSolutionSurLesCoupes( PROBLEME_SPX * Spx,
                                          int NombreDeContraintesCoupes ,int * NbTermCoupes,
                                          char * PositionDeLaVariableDEcart ) 
{
int Cnt; int il; int ilMax; int Cnt_E; char * OrigineDeLaVariable;
int * Mdeb; int * NbTerm; int * Indcol; char * PositionDeLaVariable; int Var;

NombreDeContraintesCoupes = 0; /* Pour ne pas avoir de warning a la compilation */

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;
PositionDeLaVariable = Spx->PositionDeLaVariable;
OrigineDeLaVariable  = Spx->OrigineDeLaVariable;

Cnt_E = 0;

goto NewCode;

for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  /* On cherche la variable Cnt_E correspondante */
  while ( NbTermCoupes[Cnt_E] <= 0 ) Cnt_E++;
	/* Recherche de la variable d'ecart de la coupe (ce n'est pas forcement la derniere) attention, maintenant
	   c'est la derniere donc on pourrait simplifier cela */
	il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
	  Var = Indcol[il];
	  if ( OrigineDeLaVariable[Var] == ECART ) {
      /* Variable d'ecart */
      if ( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) PositionDeLaVariableDEcart[Cnt_E] = HORS_BASE_SUR_BORNE_INF;
      else PositionDeLaVariableDEcart[Cnt_E] = EN_BASE;
	    break;
		}
		il++;
  }	
  Cnt_E++;
}
return;

NewCode:

for ( Cnt = Spx->NombreDeContraintesDuProblemeSansCoupes ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  /* On cherche la variable Cnt_E correspondante */
  while ( NbTermCoupes[Cnt_E] <= 0 ) Cnt_E++;
	/* Recherche de la variable d'ecart de la coupe c'est la derniere */
	il = Mdeb[Cnt] + NbTerm[Cnt] - 1;
  /* Variable d'ecart */
  if ( PositionDeLaVariable[Indcol[il]] == HORS_BASE_SUR_BORNE_INF ) PositionDeLaVariableDEcart[Cnt_E] = HORS_BASE_SUR_BORNE_INF;
  else PositionDeLaVariableDEcart[Cnt_E] = EN_BASE;	
  Cnt_E++;
}

return;
}

















