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

   FONCTION: Reduced cost fixing (necessite d'avoir obtenu une premiere
             solution entiere)
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

char PNE_ReducedCostFixing( PROBLEME_PNE * Pne, int * PositionDeLaVariable ) 
{
int i; double Delta; double h; char RelancerUnSimplexe; int * TypeDeBorne;
int * TypeDeVariable; double * Umin; double * Umax; double * CoutsReduits;
NOEUD * Noeud; int * NumeroDeLaVariableModifiee; char * TypeDeBorneModifiee;
double *NouvelleValeurDeBorne ; int NombreDeBornesModifiees; int NbModifs;
BB * Bb; char * T; int * TGraph; int Nb; int * Num; int j; int Arret;
int NombreDeVariablesNonFixes; int * NumeroDesVariablesNonFixes;
int NombreDeVariables; int Var; 

RelancerUnSimplexe = NON_PNE;

# if REDUCED_COST_FIXING_AUX_NOEUD_DANS_ARBRE == NON_PNE
  return( RelancerUnSimplexe );
# endif  

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeVariablesNonFixes = Pne->NombreDeVariablesNonFixes;
NumeroDesVariablesNonFixes = Pne->NumeroDesVariablesNonFixes;

TGraph = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
if ( TGraph == NULL ) return( RelancerUnSimplexe );
Num = (int *) malloc( Pne->NombreDeVariablesEntieresTrav * sizeof( int ) );
if ( Num == NULL ) {
  free( TGraph );
	return( RelancerUnSimplexe );
}
Nb = 0;
memset( (char *) TGraph, 0, Pne->NombreDeVariablesTrav * sizeof( int ) );

Bb = Pne->ProblemeBbDuSolveur;  
Noeud = Bb->NoeudEnExamen;   

T = (char *) malloc( ( Bb->NombreDeVariablesDuProbleme ) * sizeof(char) );
if ( T == NULL ) return( RelancerUnSimplexe );

memset( (char *) T, 0, Bb->NombreDeVariablesDuProbleme * sizeof( char ) );

NombreDeBornesModifiees = Noeud->NombreDeBornesModifiees;
NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;

for( i = 0 ; i < NombreDeBornesModifiees ; i++ ) {
  T[NumeroDeLaVariableModifiee[i]] = 1;
}

TypeDeBorne = Pne->TypeDeBorneTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
CoutsReduits = Pne->CoutsReduits;
 
Delta = Pne->CoutOpt - Pne->Critere;
if ( Delta < 0. ) Delta = 0.; /* Car le cout entier est toujours superieur ou egal au cout relaxe */

NbModifs = 0;

/* Premier passage: decompte des modifs de borne */
for( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];

  if ( TypeDeVariable[Var] != ENTIER ) continue;
	
  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;	
  if ( fabs( Umax[Var] - Umin[Var] ) < ZERO_VARFIXE ) continue;
  if ( fabs( CoutsReduits[Var] ) < ZERO_COUT_REDUIT ) continue;   
	if ( T[Var] != 0 ) continue;
	
	h = 0.0;
	Nb = 0;
	
  if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {
	  /* On regarde ce qu'il se passe si la variable passe a 0 */
		Arret = NON_PNE;
    PNE_ReducedCostFixingConflictGraph( Pne, Var, Umin[Var], &h, Delta, CoutsReduits, Umin, Umax, PositionDeLaVariable, &Nb, TGraph, Num, &Arret );		
		for ( j = 0 ; j < Nb ; j++ ) TGraph[Num[j]] = 0;		
    if ( h > Delta ) {		
			/* Mise a jour des bornes des contraintes */			
			NbModifs++;
		}
	}
  else if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	  /* On regarde ce qu'il se passe si la variable passe a 1 */
		Arret = NON_PNE;
    PNE_ReducedCostFixingConflictGraph( Pne, Var, Umax[Var], &h, Delta, CoutsReduits, Umin, Umax, PositionDeLaVariable, &Nb, TGraph, Num, &Arret );
    for ( j = 0 ; j < Nb ; j++ ) TGraph[Num[j]] = 0;		
    if ( h > Delta ) {		
			/* Mise a jour des bornes des contraintes */			
			NbModifs++;
		}				
	}
}

if ( NbModifs == 0 ) goto FinReducedCostFixing;

i = NombreDeBornesModifiees + NbModifs;
Noeud->NumeroDeLaVariableModifiee = (int *) realloc( Noeud->NumeroDeLaVariableModifiee, i * sizeof( int ) );
if ( Noeud->NumeroDeLaVariableModifiee == NULL ) {
  free( Noeud->TypeDeBorneModifiee );
  free( Noeud->NouvelleValeurDeBorne );
	Noeud->TypeDeBorneModifiee = NULL;
	Noeud->NouvelleValeurDeBorne = NULL;
	Noeud->NombreDeBornesModifiees = 0;
  goto FinReducedCostFixing;
}
Noeud->TypeDeBorneModifiee = (char *) realloc( Noeud->TypeDeBorneModifiee, i * sizeof( char ) );
if ( Noeud->TypeDeBorneModifiee == NULL ) {
  free( Noeud->NumeroDeLaVariableModifiee );
  free( Noeud->NouvelleValeurDeBorne );
	Noeud->NumeroDeLaVariableModifiee = NULL;
	Noeud->NouvelleValeurDeBorne = NULL;
	Noeud->NombreDeBornesModifiees = 0;
  goto FinReducedCostFixing;
}
Noeud->NouvelleValeurDeBorne = (double *) realloc( Noeud->NouvelleValeurDeBorne, i * sizeof( double ) );
if ( Noeud->NouvelleValeurDeBorne == NULL ) {
  free( Noeud->NumeroDeLaVariableModifiee );
  free( Noeud->TypeDeBorneModifiee );
	Noeud->NumeroDeLaVariableModifiee = NULL;
	Noeud->TypeDeBorneModifiee = NULL;
	Noeud->NombreDeBornesModifiees = 0;	
  goto FinReducedCostFixing;
}

NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;
TypeDeBorneModifiee  = Noeud->TypeDeBorneModifiee;
NouvelleValeurDeBorne = Noeud->NouvelleValeurDeBorne;

for( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];

  /* On ne peut pas modifier les bornes des variables continues si on est amenes calculer des coupes
	   qui se servent des bornes min et max de ces variables car alors elle ne seront plus valables
		 pour tout l'arbre */
	/* On pourrait autoriser les modifs de bornes des variables continues a condition de marquer le noeud
	   et tous ses descendants pour ne pas calculer de coupes dessus */
	
  if ( TypeDeVariable[Var] != ENTIER ) continue;
	
  if ( TypeDeBorne[Var] == VARIABLE_FIXE ) continue;
  if ( fabs( Umax[Var] - Umin[Var] ) < ZERO_VARFIXE ) continue;
  if ( fabs( CoutsReduits[Var] ) < ZERO_COUT_REDUIT ) continue;
	if ( T[Var] != 0 ) continue;
	
	h = 0.0;
	Nb = 0;
	
  if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {
	  /* On regarde ce qu'il se passe si la variable passe a 0 */
	 	Arret = NON_PNE;
    PNE_ReducedCostFixingConflictGraph( Pne, Var, Umin[Var], &h, Delta, CoutsReduits, Umin, Umax, PositionDeLaVariable, &Nb, TGraph, Num, &Arret );
    for ( j = 0 ; j < Nb ; j++ ) TGraph[Num[j]] = 0;
    if ( h > Delta ) {							
	    /* La variable entiere sera donc fixee a 1 */				
      /* Mise a jour de la borne inf */								
      Umin[Var] = Umax[Var];				
      RelancerUnSimplexe = OUI_PNE;
      NumeroDeLaVariableModifiee[NombreDeBornesModifiees] = Var;
      TypeDeBorneModifiee[NombreDeBornesModifiees] = BORNE_INF;
      NouvelleValeurDeBorne[NombreDeBornesModifiees] = Umin[Var];
			NombreDeBornesModifiees++;      
    }
	}
  else if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	  /* On regarde ce qu'il se passe si la variable passe a 1 */
	 	Arret = NON_PNE;
    PNE_ReducedCostFixingConflictGraph( Pne, Var, Umax[Var], &h, Delta, CoutsReduits, Umin, Umax, PositionDeLaVariable, &Nb, TGraph, Num, &Arret );
    for ( j = 0 ; j < Nb ; j++ ) TGraph[Num[j]] = 0;		
    if ( h > Delta ) {
	    /* La variable entiere sera donc fixee a 0 */				
      /* Mise a jour de la borne sup */
      Umax[Var] = Umin[Var];				
      RelancerUnSimplexe = OUI_PNE;
      NumeroDeLaVariableModifiee[NombreDeBornesModifiees] = Var;
      TypeDeBorneModifiee[NombreDeBornesModifiees] = BORNE_SUP;
      NouvelleValeurDeBorne[NombreDeBornesModifiees] = Umax[Var];
			NombreDeBornesModifiees++;							         
    }
	}
}

Noeud->NombreDeBornesModifiees = NombreDeBornesModifiees;

FinReducedCostFixing:
free( T );
free( TGraph );
free( Num );

/*
if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbModifs != 0 ) printf("Reduced cost fixing at current node. Variables fixed at one of its bounds: %d\n",NbModifs);
}
*/

return( RelancerUnSimplexe );
}


