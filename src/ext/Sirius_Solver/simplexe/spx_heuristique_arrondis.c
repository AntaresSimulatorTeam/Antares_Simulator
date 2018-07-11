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

   FONCTION: Pilote par la partie PNE. On fait un simplexe apres avoir
	           fait des arrondis en esperant trouver une solution entiere.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"

# include "pne_define.h"

# include "lu_fonctions.h"

# define MARGE 1.e-9

/*----------------------------------------------------------------------------*/

void SPX_HeuristiqueArrondis( 
                   PROBLEME_SPX * Spx     , 
                   int    * YaUneSolution,                  
                   int      NbVar_E ,
									 double * X_E ,
									 int    * TypeVar_E   ,
									 int      NbContr_E ,
									 int    * PositionDeLaVariable_E , 
									 int    * NbVarDeBaseComplementaires_E ,
									 int    * ComplementDeLaBase_E,									 									 
                   int      NombreMaxDIterations,
									 int      NombreDeVariablesFixees,
									 int    * NumerosDesVariablesArrondies,
									 double * NouvelleBorneMin,
									 double * NouvelleBorneMax
                            )
{
int i; int VariablePneArrondie; int VariableSpx; /* int Cnt; */ int il;
int ilMax ; double XmaxSV; double XminVariableSpx; double XmaxVariableSpx;
double ScaleLigneDesCouts; 
int * CorrespondanceVarEntreeVarSimplexe; int * ContrainteDeLaVariableEnBase; int * Cdeb; int * CNbTerm;
int * NumeroDeContrainte; double * XminEntree; double * Xmin; double * Xmax; double * ScaleX; double * X;
double * C; double * BBarre; double * B; double * ACol;

XminEntree = Spx->XminEntree;
CorrespondanceVarEntreeVarSimplexe = Spx->CorrespondanceVarEntreeVarSimplexe;

Xmin = Spx->Xmin;
Xmax = Spx->Xmax;
ScaleX = Spx->ScaleX;
X = Spx->X;
C = Spx->C;
ScaleLigneDesCouts = Spx->ScaleLigneDesCouts;
ContrainteDeLaVariableEnBase = Spx->ContrainteDeLaVariableEnBase;
BBarre = Spx->BBarre;

Cdeb = Spx->Cdeb;
CNbTerm = Spx->CNbTerm;
ACol = Spx->ACol;
NumeroDeContrainte = Spx->NumeroDeContrainte;
B = Spx->B;
			
/* Le numero fourni ne peut pas etre dans la numerotation interne au simplexe car l'appelant n'y a
   pas acces */
for ( i = 0 ; i < NombreDeVariablesFixees ; i++ ) {
  VariablePneArrondie = NumerosDesVariablesArrondies[i];
	VariableSpx = CorrespondanceVarEntreeVarSimplexe[VariablePneArrondie];
	if ( VariableSpx < 0 ) continue;
  XminEntree[VariableSpx] = NouvelleBorneMin[i];
	
  XmaxSV = Xmax[VariableSpx];
  XminVariableSpx = NouvelleBorneMin[i] / ScaleX[VariableSpx];
  XmaxVariableSpx = NouvelleBorneMax[i] / ScaleX[VariableSpx];						 

	if ( XminVariableSpx > Xmin[VariableSpx] + MARGE ) {
    /* On a demande a placer la variable sur la borne sup */
    Spx->PartieFixeDuCout += ( C[VariableSpx] * XmaxSV ) / ScaleLigneDesCouts;
    /* Il faut tenir compte de la modification du second membre B qui decoulerait du fait que la variable 
       dont on fixe borne min = borne max se retrouve avec Xmin = Xmax */
    il    = Cdeb[VariableSpx];
    ilMax = il + CNbTerm[VariableSpx];
    while ( il < ilMax ) {
      B[NumeroDeContrainte[il]]-= ACol[il] * XmaxSV;
      il++;
    }		
	}

  /* On simule la translation des bornes */
  Xmin[VariableSpx] = 0.;
  Xmax[VariableSpx] = 0.;
	
}

/* La base factorisee est la base courante et on s'arrete des qu'il faut factoriser
   la base */
Spx->SeuilDePivotDual = VALEUR_DE_PIVOT_ACCEPTABLE;
Spx->StrongBranchingEnCours = NON_SPX;
Spx->UtiliserLaLuUpdate     = OUI_SPX;
Spx->FaireDuRaffinementIteratif = 0;
Spx->FlagStabiliteDeLaFactorisation       = 0;  
Spx->ProblemeDeStabiliteDeLaFactorisation = NON_SPX;


Spx->YaUneSolution = OUI_SPX;
/*SPX_InitialiserLeTableauDesVariablesHorsBase( Spx );*/

Spx->UtiliserLaLuUpdate = OUI_SPX; 

Spx->Iteration = 0;
Spx->NombreDeChangementsDeBase = 0;
Spx->NombreMaxDIterations      = NOMBRE_MAX_DITERATIONS; 
if ( Spx->NombreMaxDIterations <= ( 5 * Spx->NombreDeContraintes ) ) {
  Spx->NombreMaxDIterations = 5 * Spx->NombreDeContraintes;
}
/* Prise en compte de la valeur fournie si elle est valide */
if ( NombreMaxDIterations > 0 ) Spx->NombreMaxDIterations = NombreMaxDIterations;

Spx->CycleDeRefactorisation = CYCLE_DE_REFACTORISATION_DUAL;

SPX_InitialiserLeTableauDesVariablesHorsBase( Spx ); /* On n'est pas oblige de le faire a chaque fois */

/* Factoriser la premiere base */
Spx->BaseInversibleDisponible = NON_SPX;
SPX_FactoriserLaBase( Spx );

if ( Spx->YaUneSolution != NON_SPX ) SPX_DualSimplexe( Spx );

if ( Spx->YaUneSolution == OUI_SPX ) {

  /* Recalcul systematique des variables duales car elle ne sont pas mise a jour a chaque iteration */
  /* Calcul de Pi = c_B * B^{-1} */
  SPX_CalculerPi( Spx );          
 
  /* On initialise les valeurs des variables en fonction de leur position */
  SPX_FixerXEnFonctionDeSaPosition( Spx );
  
  /* Dans le cas d'un contexte de Branch And Bound ou de Branch And Cut on sauvegarde les donnees 
     qui seront necessaires pour faire du strong branching voire des coupes de Gomory. Si la base 
     ne vient pas d'être factorisée, on la factorise. */
  /*if ( Spx->Contexte != SIMPLEXE_SEUL ) SPX_SauvegardesBranchAndBoundAndCut( Spx );*/   
  SPX_RecupererLaSolution( Spx , NbVar_E , X_E , TypeVar_E   , NbContr_E , PositionDeLaVariable_E , 
                           NbVarDeBaseComplementaires_E , ComplementDeLaBase_E );
  }

*YaUneSolution  = Spx->YaUneSolution; 

/* Attention dans RecupererLaSolution on a fait un unscaling. Il faut donc remettre les valeur de X car
   elles sont reutilisees */

if ( Spx->FaireDuScalingSPX == OUI_SPX ) {
  ScaleX  = Spx->ScaleX;
  for ( i = 0 ; i < Spx->NombreDeVariables ; i++ ) X[i] /= ScaleX[i];
}

return;

}

/*----------------------------------------------------------------------------*/

void SPX_RAZHeuristiqueArrondis( PROBLEME_SPX * Spx )
{
int il;
/* On remet les donnees internes du simplexe dans l'etat initial */
il = Spx->NombreDeVariables * sizeof( double );
memcpy( (char *) Spx->X, (char *) Spx->XSV, il );
memcpy( (char *) Spx->CBarre, (char *) Spx->CBarreSV, il );
memcpy( (char *) Spx->C, (char *) Spx->Csv, il );
il = Spx->NombreDeVariables * sizeof( int );
memcpy( (char *) Spx->ContrainteDeLaVariableEnBase, (char *) Spx->ContrainteDeLaVariableEnBaseSV, il );
il = Spx->NombreDeVariables * sizeof( char );
memcpy( (char *) Spx->PositionDeLaVariable, (char *) Spx->PositionDeLaVariableSV, il );
memcpy( (char *) Spx->InDualFramework, (char *) Spx->InDualFrameworkSV, il );

il = Spx->NombreDeContraintes * sizeof( double );
memcpy( (char *) Spx->BBarre, (char *) Spx->BBarreSV, il );
memcpy( (char *) Spx->DualPoids, (char *) Spx->DualPoidsSV, il );
il = Spx->NombreDeContraintes * sizeof( int );
memcpy( (char *) Spx->VariableEnBaseDeLaContrainte, (char *) Spx->VariableEnBaseDeLaContrainteSV, il );
memcpy( (char *) Spx->CdebBase, (char *) Spx->CdebBaseSV, il );
memcpy( (char *) Spx->NbTermesDesColonnesDeLaBase, (char *) Spx->NbTermesDesColonnesDeLaBaseSV, il );

Spx->LastEta = -1;
Spx->NombreDeChangementsDeBase = 0;
Spx->Iteration                 = 0;



return;
}
