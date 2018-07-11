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
   FONCTION: Modification du probleme dans un contexte de branch and 
             bound. En effet, d'un probleme a un autre, la seule 
             difference consiste en un changement de borne pour la 
             variable instanciee. En consequence il n'est pas necessaire 
             de reconstruire l'ensemble du probleme (scaling et autres 
             chainages de la matrice des contraintes).
             Les donnees qui changent sont:
             * Les bornes des variables entieres.
             * La base (ici la base de depart est toujours fournie).
             Attention, les types de variables (fixe ou pas) ne doivent 
             pas changer, les couts non plus.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"  
# include "spx_define.h"

/*----------------------------------------------------------------------------*/

void SPX_ModifierLeProbleme(
         PROBLEME_SPX * Spx,
         double * C_E,        /* Couts lineaires */
         double * X_E,        /* Inconnues */
         double * Xmin_E,     /* Borne inf des variables */
         double * Xmax_E,     /* Borne sup des variables */
         int     NbVar_E,    /* Nombre de variables */
         int   * TypeVar_E,  /* Indicateur du type de variable, il ne doit prendre que les 
                                 suivantes (voir le fichier spx_define.h mais ne jamais utiliser 
                                 les valeurs explicites des constantes): 
                                  VARIABLE_FIXE                  , 
                                  VARIABLE_BORNEE_DES_DEUX_COTES , 
                                  VARIABLE_BORNEE_INFERIEUREMENT , 
                                  VARIABLE_BORNEE_SUPERIEUREMENT , 	
                                  VARIABLE_NON_BORNEE     	  
                              */
         /* Pilotage de l'algorithme dual */
	       double CoutMax,
         int   UtiliserCoutMax
                              ) 
{  
int il ; int ilMax; int Var_E  ; int Var; int Cnt  ; double Marge; double S;
double UnSurScaleXFoisUnSurSupXmax; int * CorrespondanceVarEntreeVarSimplexe;

char * TypeDeVariable; double * X; double * Xmin; double * Xmax   ; double * C  ;
double * XminEntree  ; double * XmaxEntree      ; double * XEntree; double * B  ;
double * BAvantTranslationEtApresScaling        ; double * A      ; int * Mdeb ;
int * NbTerm        ; int * Indcol            ; double * ScaleX ; double * Csv;
double PartieFixeDuCout;

/*
printf("SPX_ModifierLeProbleme \n"); fflush(stdout);
*/

Spx->NombreDeVariables   = Spx->NombreDeVariablesDuProblemeSansCoupes;
Spx->NombreDeContraintes = Spx->NombreDeContraintesDuProblemeSansCoupes;

Spx->FaireScalingLU     = 0;
Spx->UtiliserCoutMax    = NON_SPX;
if ( Spx->AlgorithmeChoisi == SPX_DUAL && UtiliserCoutMax == OUI_SPX ) { 
  Marge = 1.e-6 * fabs ( CoutMax );
  if ( Marge < 1.e-6 ) Marge = 1.e-6;
  if ( Marge > 1.e+1 ) Marge = 1.e+1;
  Spx->CoutMax         = CoutMax + Marge;
  Spx->UtiliserCoutMax = OUI_SPX;
}

PartieFixeDuCout = 0.;
if ( Spx->UtiliserCoutMax == OUI_SPX ) {
  for ( Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {
    if ( TypeVar_E[Var_E] == VARIABLE_FIXE ) {
      PartieFixeDuCout+= C_E[Var_E] * X_E[Var_E];
      continue;
    }
    if ( TypeVar_E[Var_E] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeVar_E[Var_E] == VARIABLE_BORNEE_INFERIEUREMENT ) {               
      PartieFixeDuCout+= C_E[Var_E] * Xmin_E[Var_E];
    }
  }
}
Spx->PartieFixeDuCout = PartieFixeDuCout;

/*------------------------------------------------------------*/

CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;
TypeDeVariable = Spx->TypeDeVariable;
X    = Spx->X;
Xmin = Spx->Xmin;
Xmax = Spx->Xmax;
C    = Spx->C;
Csv  = Spx->Csv;

XminEntree = Spx->XminEntree;
XmaxEntree = Spx->XmaxEntree; 
XEntree    = Spx->XEntree;

BAvantTranslationEtApresScaling = Spx->BAvantTranslationEtApresScaling;
B      = Spx->B;
Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
A      = Spx->A;
Indcol = Spx->Indcol;
ScaleX = Spx->ScaleX;

/* Initialisation sur les variables */
for ( Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {
  Var = CorrespondanceVarEntreeVarSimplexe[Var_E];
  if ( Var < 0 || Var >= Spx->NombreDeVariables ) continue;
  X[Var] = X_E[Var_E];
  /* Nouvelles bornes */
  Xmin[Var] = Xmin_E[Var_E];
  Xmax[Var] = Xmax_E[Var_E];  
  C   [Var] = Csv[Var];
  /* Comme on peut changer le type a cause du reduced cost fixing on le met a jour */
  if ( TypeVar_E[Var_E] == VARIABLE_BORNEE_DES_DEUX_COTES )      TypeDeVariable[Var] = BORNEE;
  else if ( TypeVar_E[Var_E] == VARIABLE_BORNEE_INFERIEUREMENT ) TypeDeVariable[Var] = BORNEE_INFERIEUREMENT;
  else if ( TypeVar_E[Var_E] == VARIABLE_NON_BORNEE )            TypeDeVariable[Var] = NON_BORNEE;
  else {
    printf("Bug dans l'appel du Simplexe, variable simplexe %d variable externe %d \n",Var,Var_E);
    printf("    type de borne %d non autorise en mode branch and bound\n",(int) TypeVar_E[Var_E]);
    exit(0);   
  }  
}

/* Les bornes min. des variables entieres ont pu etre relevees pour une instanciation a 1.
   Il faut donc refaire une translation des bornes. */
/* Translation des bornes */
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {  
  XminEntree[Var] = Xmin[Var];
  XmaxEntree[Var] = Xmax[Var]; 
  if ( TypeDeVariable[Var] == NON_BORNEE ) continue;
  if ( TypeDeVariable[Var] == BORNEE ) Xmax[Var] = XmaxEntree[Var] - XminEntree[Var];
  Xmin[Var] = 0.;
  XEntree[Var] = X[Var];
  X[Var]       = XEntree[Var] - XminEntree[Var];
}

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) {
  B[Cnt] = BAvantTranslationEtApresScaling[Cnt];
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  S     = 0.;
  while ( il < ilMax ) {
    Var = Indcol[il];
    if ( TypeDeVariable[Var] != NON_BORNEE ) S+= A[il] * XminEntree[Var] / ScaleX[Var];
    il++;
  }
  B[Cnt]-=S;
}

/* Scaling des bornes */
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) {
  /* Attention, on suppose toujours que la variable est bornee */
  UnSurScaleXFoisUnSurSupXmax = 1. / ScaleX[Var];
  X[Var]*= UnSurScaleXFoisUnSurSupXmax;
  if ( TypeDeVariable[Var] != NON_BORNEE ) Xmin[Var]*= UnSurScaleXFoisUnSurSupXmax;
  if ( TypeDeVariable[Var] == BORNEE     ) Xmax[Var]*= UnSurScaleXFoisUnSurSupXmax;
}

return;
}

/*----------------------------------------------------------------------------*/
/* Construction de la base de depart (hors coupes) */

void SPX_ConstruireLaBaseDuProblemeModifie( PROBLEME_SPX * Spx, int NbVar_E,
                                            int * PositionDeLaVariable_E,
                                            int   NbVarDeBaseComplementaires_E,  
                                            int * ComplementDeLaBase_E )		     
{
int Cnt; int Var; int Var_E; int il; /*int ilMax;*/ int * VariableEnBaseDeLaContrainte;
char * PositionDeLaVariable; int * CorrespondanceCntEntreeCntSimplexe;
int * ContrainteDeLaVariableEnBase; int * Mdeb; int * NbTerm; int * Indcol;
char * OrigineDeLaVariable;

NbVar_E = 0; /* Pour ne pas avoir de warning a la compilation */
PositionDeLaVariable = (char *) PositionDeLaVariable_E; /* Pour ne pas avoir de warning a la compilation */

VariableEnBaseDeLaContrainte       = Spx->VariableEnBaseDeLaContrainte;
PositionDeLaVariable               = Spx->PositionDeLaVariable;
CorrespondanceCntEntreeCntSimplexe = Spx->CorrespondanceCntEntreeCntSimplexe;
ContrainteDeLaVariableEnBase       = Spx->ContrainteDeLaVariableEnBase;
OrigineDeLaVariable                = Spx->OrigineDeLaVariable;

for ( Cnt = 0 ; Cnt < Spx->NombreDeContraintes ; Cnt++ ) VariableEnBaseDeLaContrainte[Cnt] = -1;
for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ )   PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;

Mdeb   = Spx->Mdeb;
NbTerm = Spx->NbTerm;
Indcol = Spx->Indcol;

for ( Var_E = 0 ; Var_E < NbVarDeBaseComplementaires_E ; Var_E++ ) {
  Cnt = CorrespondanceCntEntreeCntSimplexe[ComplementDeLaBase_E[Var_E]]; 
  if ( Cnt < 0 ) {
	  printf("Attention, anomalie detectee dans SPX_ConstruireLaBaseDuProblemeModifie:\n");
		printf("  La variable d'ecart contrainte %d est fournie comme etant basique or cette contrainte est inconnue du simplexe\n",Var_E);
	  continue;
	}
	/* Comme le chainage est reorganise en cours d'algorithme, il faut rechercher la variable dont
	   l'origine est ECART ou BASIQUE_ARTIFICIELLE */
	/* Non: on est revenu a l'ancienne methode. La variable d'ecart ou la variable BASIQUE_ARTIFICIELLE est
	   toujours la derniere de la contrainte */
	/*
	il = Mdeb[Cnt];
	ilMax = il + NbTerm[Cnt];
	while ( il < ilMax ) {
	  Var = Indcol[il];
	  if ( OrigineDeLaVariable[Var] == ECART ||	OrigineDeLaVariable[Var] == BASIQUE_ARTIFICIELLE ) {
      PositionDeLaVariable        [Var] = EN_BASE_LIBRE;
      ContrainteDeLaVariableEnBase[Var] = Cnt; 
      VariableEnBaseDeLaContrainte[Cnt] = Var;
	    break;
		}
	 	il++;
  }
	*/
	il = Mdeb[Cnt] + NbTerm[Cnt] - 1;
	Var = Indcol[il];
  PositionDeLaVariable        [Var] = EN_BASE_LIBRE;
  ContrainteDeLaVariableEnBase[Var] = Cnt; 
  VariableEnBaseDeLaContrainte[Cnt] = Var;		
}

return;
} 

/*----------------------------------------------------------------------------*/

void SPX_CompleterLaBaseDuProblemeModifie(
         PROBLEME_SPX * Spx,
         int     NbVar_E,                  /* Nombre de variables */
         int     NbContr_E,                /* Nombre de contraintes  */
	       int   * PositionDeLaVariable_E    /* La base sur les variables natives */
                              ) 
{
int j; int Var_E; int Var ; char * CorrectionDuale; char * TypeDeVariable;
int * CorrespondanceVarEntreeVarSimplexe; char * PositionDeLaVariable;
int * ContrainteDeLaVariableEnBase; int * VariableEnBaseDeLaContrainte;

NbContr_E = 0; /* Pour ne pas avoir de warning a la compilation */

/* Reinitialisation des corrections duales une fois qu'on a toutes les variables 
   sous la main */

CorrectionDuale = Spx->CorrectionDuale;

for ( Var = 0 ; Var < Spx->NombreDeVariables ; Var++ ) CorrectionDuale[Var] = NOMBRE_MAX_DE_PERTURBATIONS_DE_COUT;     

/* Pour les contraintes et les variables restantes */

CorrespondanceVarEntreeVarSimplexe =  Spx->CorrespondanceVarEntreeVarSimplexe;
PositionDeLaVariable               =  Spx->PositionDeLaVariable;
ContrainteDeLaVariableEnBase       =  Spx->ContrainteDeLaVariableEnBase;
VariableEnBaseDeLaContrainte       =  Spx->VariableEnBaseDeLaContrainte;
TypeDeVariable                     =  Spx->TypeDeVariable;

for ( j = 0 , Var_E = 0 ; Var_E < NbVar_E ; Var_E++ ) {

  Var = CorrespondanceVarEntreeVarSimplexe[Var_E];

  if ( Var < 0 || Var >= Spx->NombreDeVariables ) continue;	
  if ( PositionDeLaVariable_E[Var_E] == EN_BASE ) {
    PositionDeLaVariable[Var] = EN_BASE_LIBRE;
    while ( VariableEnBaseDeLaContrainte[j] != - 1 ) j++;  
    ContrainteDeLaVariableEnBase[Var] = j; 
    VariableEnBaseDeLaContrainte[j]   = Var;		
    j++;
  }
  else if ( PositionDeLaVariable_E[Var_E] == HORS_BASE_SUR_BORNE_INF ) {
	  PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
		/* Mise en coherence en cas de changement de type de borne dans la partie PNE */
		if ( TypeDeVariable[Var] == NON_BORNEE ) PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
	}
  else if ( PositionDeLaVariable_E[Var_E] == HORS_BASE_SUR_BORNE_SUP ) {
	  PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_SUP;
		/* Mise en coherence en cas de changement de type de borne dans la partie PNE */
		if ( TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
		else if ( TypeDeVariable[Var] == NON_BORNEE ) PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
	}
  else if ( PositionDeLaVariable_E[Var_E] == HORS_BASE_A_ZERO ) {
	  PositionDeLaVariable[Var] = HORS_BASE_A_ZERO;
		/* Mise en coherence en cas de changement de type de borne dans la partie PNE */
		if ( TypeDeVariable[Var] == BORNEE || TypeDeVariable[Var] == BORNEE_INFERIEUREMENT ) {
		  PositionDeLaVariable[Var] = HORS_BASE_SUR_BORNE_INF;
		}		
	}
  else {
    printf(" Bug dans la fourniture de la base de depart, la variable %d est mal positionnee\n",Var_E);
    printf(" son positionnement donne est %d \n",(int) PositionDeLaVariable_E[Var_E]);
    exit(0);
  }
}

/* Pour ne pas utiliser les poids precedents */
Spx->LeSteepestEdgeEstInitilise = NON_SPX;

return;
}
