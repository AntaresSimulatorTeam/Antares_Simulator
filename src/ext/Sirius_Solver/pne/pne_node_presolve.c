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

   FONCTION: On etudie les domaines de variation des variables entieres
	           dans le but d'en fixer.
                
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

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PNE_NodePresolve( PROBLEME_PNE * Pne, int * Faisabilite )   
{
int Var; double * Umin; double * Umax; int NombreDeVariables; double Ai;
int il; double * U; int NombreDeContraintes; double * UminSv; double * UmaxSv;
char * BorneInfConnue; double * ValeurDeBorneInf; double * Bmin; double * Bmax;
int * NumeroDesVariablesFixees; int i; char CodeRet; BB * Bb; NOEUD * Noeud;
char * T; int NombreDeBornesModifiees; int * NumeroDeLaVariableModifiee; char Mode;
char * TypeDeBorneModifiee; double * NouvelleValeurDeBorne; CONFLICT_GRAPH * ConflictGraph;
PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; char * ContrainteActivable;

Bb = Pne->ProblemeBbDuSolveur; 
Noeud = Bb->NoeudEnExamen;

if ( Noeud->ProfondeurDuNoeud < PROFONDEUR_MIN_POUR_NODE_PRESOLVE && Noeud->ProfondeurDuNoeud > 1 ) return;

if ( Noeud->ProfondeurDuNoeud % CYCLE_NODE_PRESOLVE != 0 ) return;
    
/*printf("PNE_NodePresolve \n");*/

/* Dans la suite: node presolve simplifie */

ConflictGraph = Pne->ConflictGraph;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
NombreDeVariables = Pne->NombreDeVariablesTrav;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
U = Pne->UTrav;

Mode = PRESOLVE_SIMPLIFIE_POUR_NODE_PRESOLVE;

if ( Pne->ProbingOuNodePresolve == NULL ) {
  PNE_ProbingNodePresolveAlloc( Pne, &CodeRet );
  if ( CodeRet == NON_PNE ) return;
}

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

ProbingOuNodePresolve->Faisabilite = OUI_PNE;
ProbingOuNodePresolve->VariableInstanciee = -1;
ProbingOuNodePresolve->NbVariablesModifiees = 0;
ProbingOuNodePresolve->NbContraintesModifiees = 0;

BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;

NumeroDesVariablesFixees = ProbingOuNodePresolve->NumeroDesVariablesFixees;
ProbingOuNodePresolve->NombreDeVariablesFixees = 0;

T = NULL;
ContrainteActivable = NULL;

ContrainteActivable = (char *) malloc( NombreDeContraintes * sizeof( char ) );
if ( ContrainteActivable == NULL ) return;
memcpy( (char *) ContrainteActivable, (char *) Pne->ContrainteActivable, NombreDeContraintes * sizeof( char ) );

PNE_InitBorneInfBorneSupDesVariables( Pne );
																	
/* Calcul min et max des contraintes */
/* Au noeud racine on part "from scratch" et on archive le resultat, il faut le faire a chaque passage car on a pu fixer des bornes */
if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) {
  PNE_CalculMinEtMaxDesContraintes( Pne, Faisabilite );
  if ( *Faisabilite == NON_PNE ) {	
 	  # if TRACES == 1
		  printf("Pas de solution dans le presolve apres CalculMinEtMaxDesContraintes\n");
	  # endif 
    goto Fin;
  }	
	/* Et on sauvegarde le resultat comme point de depart pour les noeuds suivants */
  memcpy( (char *) ProbingOuNodePresolve->BminSv, (char *) ProbingOuNodePresolve->Bmin, NombreDeContraintes * sizeof( double ) );
  memcpy( (char *) ProbingOuNodePresolve->BmaxSv, (char *) ProbingOuNodePresolve->Bmax, NombreDeContraintes * sizeof( double ) );	
}
else {
  /* Aux autres noeuds, on part des valeurs au noeud racine et on tient compte des instanciations
     et des nouvelles bornes */
  PNE_NodePresolveInitBornesDesContraintes( Pne, Faisabilite );
  if ( *Faisabilite == NON_PNE ) {	
 	  # if TRACES == 1
		  printf("Pas de solution dans le presolve apres NodePresolveInitBornesDesContraintes\n");
	  # endif 
    goto Fin;
  }	
}

PNE_PresolveSimplifie( Pne, ContrainteActivable, Mode, Faisabilite );

Fin:

if ( *Faisabilite == NON_PNE ) {
  # if TRACES == 1
    printf("Pas de solution dans le node presolve\n");
  # endif	
  goto FreeMemoire;
}
if ( ProbingOuNodePresolve->NombreDeVariablesFixees <= 0 ) goto FreeMemoire;

if ( Bb->NoeudEnExamen == Bb->NoeudRacine ) goto ModifsBornesAuNoeudRacine;	

T = (char *) malloc( ( Bb->NombreDeVariablesDuProbleme ) * sizeof(char) );
if ( T == NULL ) goto FreeMemoire;
memset( (char *) T, 0, Bb->NombreDeVariablesDuProbleme * sizeof( char ) );
NombreDeBornesModifiees = Noeud->NombreDeBornesModifiees;
NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;
for( i = 0 ; i < NombreDeBornesModifiees ; i++ ) T[NumeroDeLaVariableModifiee[i]] = 1;

il = NombreDeBornesModifiees + ProbingOuNodePresolve->NombreDeVariablesFixees;
Noeud->NumeroDeLaVariableModifiee = (int *) realloc( Noeud->NumeroDeLaVariableModifiee, il * sizeof( int ) );
if ( Noeud->NumeroDeLaVariableModifiee == NULL ) {
  free( Noeud->TypeDeBorneModifiee );
  free( Noeud->NouvelleValeurDeBorne );
	Noeud->TypeDeBorneModifiee = NULL;
	Noeud->NouvelleValeurDeBorne = NULL;
	Noeud->NombreDeBornesModifiees = 0;
  goto FreeMemoire;
}
Noeud->TypeDeBorneModifiee = (char *) realloc( Noeud->TypeDeBorneModifiee, il * sizeof( char ) );
if ( Noeud->TypeDeBorneModifiee == NULL ) {
  free( Noeud->NumeroDeLaVariableModifiee );
  free( Noeud->NouvelleValeurDeBorne );
	Noeud->NumeroDeLaVariableModifiee = NULL;
	Noeud->NouvelleValeurDeBorne = NULL;
	Noeud->NombreDeBornesModifiees = 0;
  goto FreeMemoire;
}
Noeud->NouvelleValeurDeBorne = (double *) realloc( Noeud->NouvelleValeurDeBorne, il * sizeof( double ) );
if ( Noeud->NouvelleValeurDeBorne == NULL ) {
  free( Noeud->NumeroDeLaVariableModifiee );
  free( Noeud->TypeDeBorneModifiee );   
	Noeud->NumeroDeLaVariableModifiee = NULL;
	Noeud->TypeDeBorneModifiee = NULL;
	Noeud->NombreDeBornesModifiees = 0;	
  goto FreeMemoire;
}

NumeroDeLaVariableModifiee = Noeud->NumeroDeLaVariableModifiee;
TypeDeBorneModifiee = Noeud->TypeDeBorneModifiee;
NouvelleValeurDeBorne = Noeud->NouvelleValeurDeBorne;

/* De plus on initialise les bornes pour le simplexe qui va suivre. Mais on ne le fait que pour les
   variables entieres */

for ( i = 0 ; i < ProbingOuNodePresolve->NombreDeVariablesFixees ; i++ ) {
  Var = NumeroDesVariablesFixees[i];	
	Ai = ValeurDeBorneInf[Var];
	U[Var] = Ai;
  Umin[Var] = Ai;
	Umax[Var] = Ai;	
	if ( BorneInfConnue[Var] == FIXATION_SUR_BORNE_SUP ) {	
    NumeroDeLaVariableModifiee[NombreDeBornesModifiees] = Var;
    TypeDeBorneModifiee[NombreDeBornesModifiees] = BORNE_INF;
		NouvelleValeurDeBorne[NombreDeBornesModifiees] = Umin[Var];
	  NombreDeBornesModifiees++;
		T[Var] = 2;
	}
	else if (  BorneInfConnue[Var] == FIXATION_SUR_BORNE_INF ) {	
    NumeroDeLaVariableModifiee[NombreDeBornesModifiees] = Var;
    TypeDeBorneModifiee[NombreDeBornesModifiees] = BORNE_SUP;
		NouvelleValeurDeBorne[NombreDeBornesModifiees] = Umax[Var];
	  NombreDeBornesModifiees++;
		T[Var] = 2;
	}
	else {
	  printf("BUG dans le node presolve: code BorneInfConnue = %d incorrect\n",BorneInfConnue[Var]);
		exit(0);
	}
}
Noeud->NombreDeBornesModifiees = NombreDeBornesModifiees;

FreeMemoire:

free( T );
free( ContrainteActivable );

return;

ModifsBornesAuNoeudRacine:

UminSv = Pne->UminTravSv;
UmaxSv = Pne->UmaxTravSv;

for ( i = 0 ; i < ProbingOuNodePresolve->NombreDeVariablesFixees ; i++ ) {
  Var = NumeroDesVariablesFixees[i];
	Ai = ValeurDeBorneInf[Var];
	U[Var] = Ai;
  Umin[Var] = Ai;
	Umax[Var] = Ai;
	UminSv[Var] = Ai;
	UmaxSv[Var] = Ai;
}

/* Attention: il faut recalculer les Min et Max des contraintes si des bornes sont modifiees */
if ( ProbingOuNodePresolve->NombreDeVariablesFixees > 0 ) {
  PNE_InitBorneInfBorneSupDesVariables( Pne );
  PNE_CalculMinEtMaxDesContraintes( Pne, Faisabilite );
  if ( *Faisabilite == NON_PNE ) {	
 	  # if TRACES == 1
		  printf("Pas de solution apres le presolve du noeud racine\n");
	  # endif 
    goto Fin;
  }	
	/* Et on sauvegarde le resultat comme point de depart pour les noeuds suivants */
  memcpy( (char *) ProbingOuNodePresolve->BminSv, (char *) ProbingOuNodePresolve->Bmin, NombreDeContraintes * sizeof( double ) );
  memcpy( (char *) ProbingOuNodePresolve->BmaxSv, (char *) ProbingOuNodePresolve->Bmax, NombreDeContraintes * sizeof( double ) );	
}

return;
}


