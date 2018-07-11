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

   FONCTION: Variable probing
                
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

# define VERBOSE_VARIABLE_PROBING 0 

# define DEBUG NON_PNE /* NON_PNE */

# define MAJ_FLAG OUI_PNE /*OUI_PNE*/

/* Remarque: il y a aussi une limite liee au nombre de balayages de la matrice */
# define DUREE_DU_PROBING 1800 /*2*/ /* En seconde */
# define NB_MAX_PARCOURS_MATRICE (2*1000)

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingSauvegardesDonnees( PROBLEME_PNE * Pne )
{
int NombreDeVariables; int NombreDeContraintes;
PROBING_OU_NODE_PRESOLVE * Prb;
Prb = Pne->ProbingOuNodePresolve;
NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

memcpy( (char *) Prb->BorneInfConnueSv, (char *) Prb->BorneInfConnue, NombreDeVariables * sizeof( char ) );
memcpy( (char *) Prb->BorneSupConnueSv, (char *) Prb->BorneSupConnue, NombreDeVariables * sizeof( char ) );
memcpy( (char *) Prb->ValeurDeBorneInfSv, (char *) Prb->ValeurDeBorneInf, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Prb->ValeurDeBorneSupSv, (char *) Prb->ValeurDeBorneSup, NombreDeVariables * sizeof( double ) );

memcpy( (char *) Prb->BminSv, (char *) Prb->Bmin, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Prb->BmaxSv, (char *) Prb->Bmax, NombreDeContraintes * sizeof( double ) );

Prb->NbVariablesModifiees = 0;
Prb->NbContraintesModifiees = 0;
memset( (char *) Prb->VariableModifiee, NON_PNE, NombreDeVariables   * sizeof( char ) );
memset( (int *) Prb->NbFoisContrainteModifiee, 0, NombreDeContraintes * sizeof( int ) );

Prb->NbCntCoupesDeProbing = 0;
memset( (char *) Prb->FlagCntCoupesDeProbing, 0, NombreDeContraintes * sizeof( char ) );

return;
}

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingReinitDonnees( PROBLEME_PNE * Pne )
{
int NombreDeVariables; int NombreDeContraintes; int Var; int Cnt; int i; int * NumeroDeVariableModifiee;
char * VariableModifiee; int * NumeroDeContrainteModifiee; int * NbFoisContrainteModifiee;
char * BorneInfConnue; char * BorneInfConnueSv; char * BorneSupConnue; char * BorneSupConnueSv;
double * ValeurDeBorneInf; double * ValeurDeBorneInfSv; double * ValeurDeBorneSup;
double * ValeurDeBorneSupSv; double * Bmin; double * BminSv; double * Bmax; double * BmaxSv;
PROBING_OU_NODE_PRESOLVE * Prb;
Prb = Pne->ProbingOuNodePresolve;
NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;

/* Recuperation des variables */
/*
memcpy( (char *) Prb->BorneInfConnue, (char *) Prb->BorneInfConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) Prb->BorneSupConnue, (char *) Prb->BorneSupConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) Prb->ValeurDeBorneInf, (char *) Prb->ValeurDeBorneInfSv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Prb->ValeurDeBorneSup, (char *) Prb->ValeurDeBorneSupSv, NombreDeVariables * sizeof( double ) );
*/
NumeroDeVariableModifiee = Prb->NumeroDeVariableModifiee;
VariableModifiee = Prb->VariableModifiee;
BorneInfConnue = Prb->BorneInfConnue;
BorneInfConnueSv = Prb->BorneInfConnueSv;
BorneSupConnue = Prb->BorneSupConnue;
BorneSupConnueSv = Prb->BorneSupConnueSv;
ValeurDeBorneInf = Prb->ValeurDeBorneInf;
ValeurDeBorneInfSv = Prb->ValeurDeBorneInfSv;
ValeurDeBorneSup = Prb->ValeurDeBorneSup;
ValeurDeBorneSupSv = Prb->ValeurDeBorneSupSv;
for ( i = 0 ; i < Prb->NbVariablesModifiees ; i++ ) {
  Var = NumeroDeVariableModifiee[i];
  BorneInfConnue[Var] = BorneInfConnueSv[Var];
  BorneSupConnue[Var] = BorneSupConnueSv[Var];
	ValeurDeBorneInf[Var] = ValeurDeBorneInfSv[Var]; 
  ValeurDeBorneSup[Var] =	ValeurDeBorneSupSv[Var];
	VariableModifiee[Var] = NON_PNE;
}
Prb->NbVariablesModifiees = 0;

# if DEBUG == OUI_PNE
  for ( i = 0 ; i < NombreDeVariables ; i++ ) {
    if ( BorneInfConnue[i] != BorneInfConnueSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 1\n");
			exit(0);
		}
    if ( BorneSupConnue[i] != BorneSupConnueSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 2\n");
			exit(0);
		}
	  if (ValeurDeBorneInf[i] != ValeurDeBorneInfSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 3\n");
			exit(0);
		}
    if (ValeurDeBorneSup[i] !=	ValeurDeBorneSupSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 4\n");
			exit(0);
		}
	}
# endif

/* Recuperation des contraintes */
/*
memcpy( (char *) Prb->Bmin, (char *) Prb->BminSv, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Prb->Bmax, (char *) Prb->BmaxSv, NombreDeContraintes * sizeof( double ) );
*/
NumeroDeContrainteModifiee = Prb->NumeroDeContrainteModifiee;
NbFoisContrainteModifiee = Prb->NbFoisContrainteModifiee;
Bmin = Prb->Bmin;
BminSv = Prb->BminSv;
Bmax = Prb->Bmax;
BmaxSv = Prb->BmaxSv;
for ( i = 0 ; i < Prb->NbContraintesModifiees ; i++ ) {
  Cnt = NumeroDeContrainteModifiee[i];
	Bmin[Cnt] = BminSv[Cnt];
	Bmax[Cnt] = BmaxSv[Cnt];
  NbFoisContrainteModifiee[Cnt] = 0;
}
Prb->NbContraintesModifiees = 0;

# if DEBUG == OUI_PNE
  for ( i = 0 ; i < NombreDeContraintes ; i++ ) {
    if ( Bmin[i] != BminSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 5\n");
			exit(0);
		}
    if ( Bmax[i] != BmaxSv[i] ) {
      printf("Bug dans PNE_VariableProbingReinitDonnees 6\n");
			exit(0);
		}
	}
# endif

Prb->NombreDeVariablesFixees = 0; /* Sert a construire le graphe de conflit */

# if DEBUG == OUI_PNE
  if ( Prb->NombreDeContraintesAAnalyser > 0 ) {
    printf("Erreur dans PNE_VariableProbingReinitDonnees: NombreDeContraintesAAnalyser doit etre nul et vaut %d\n",Prb->NombreDeContraintesAAnalyser);
		exit(0);
  }
  if ( Prb->NbCntCoupesDeProbing > 0 ) {
    printf("Erreur dans PNE_VariableProbingReinitDonnees: NbCntCoupesDeProbing doit etre nul et vaut %d\n",Prb->NbCntCoupesDeProbing);
		exit(0);
  }

  for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
	  if ( Prb->FlagCntCoupesDeProbing[Cnt] != 0 ) {
      printf("Erreur dans PNE_VariableProbingReinitDonnees: FlagCntCoupesDeProbing[%d] doit etre nul\n",Cnt);
		  exit(0);
		}
	  if ( Prb->ContrainteAAnalyser[Cnt] != NON_PNE ) {
      printf("Erreur dans PNE_VariableProbingReinitDonnees: ContrainteAAnalyser[%d] doit etre nul egal a NON_PNE\n",Cnt);
		  exit(0);
		}	
	}	
# endif

Prb->NombreDeContraintesAAnalyser = 0;
Prb->NbCntCoupesDeProbing = 0;

return;
}
   
/*----------------------------------------------------------------------------*/
/* On fait du variable probing en faisant du node presolve apres fixation
   de variables */
void PNE_VariableProbingPreparerInstanciation( PROBLEME_PNE * Pne, int Var, double ValeurDeVar )
{
int Edge; int Noeud; int Complement; int Nv; int Pivot; double * ValeurDeBorneSup; double * ValeurDeBorneInf;
char * BorneInfConnue; char * BorneSupConnue; int * First; int * Adjacent; int * Next;
char BorneMiseAJour; char UneVariableAEteFixee;

BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;
  
BorneMiseAJour = NON_PNE;

PNE_InitListeDesContraintesAExaminer( Pne, Var, ValeurDeVar, BorneMiseAJour );
if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;

if ( ValeurDeVar == 1 ) UneVariableAEteFixee = FIXE_AU_DEPART;			
else if ( ValeurDeVar == 0 ) UneVariableAEteFixee = FIXE_AU_DEPART;			
else { printf("BUG dans VariableProbingPreparerInstanciation\n"); return; }

PNE_MajIndicateursDeBornes(  Pne, ValeurDeBorneInf, ValeurDeBorneSup, BorneInfConnue, BorneSupConnue,
														 ValeurDeVar, Var, UneVariableAEteFixee, BorneMiseAJour );
														
/* On applique le conflict graph s'il existe */
if ( Pne->ConflictGraph == NULL ) return;

Pivot = Pne->ConflictGraph->Pivot;
First = Pne->ConflictGraph->First;
Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next;

if ( ValeurDeVar == 1.0 ) { Noeud = Var; Complement = Pivot + Var; }
else { Noeud = Pivot + Var; Complement = Var; }

Edge = First[Noeud];
while ( Edge >= 0 ) {
  Nv = Adjacent[Edge];
	/* Attention a ne pas prendre le complement */
	if ( Nv == Complement ) goto NextEdge;
  if ( Nv < Pivot ) {
	  Var = Nv;
		/* On ne doit pas avoir U[Var] = 1.0 */
		if ( ValeurDeBorneInf[Var] > 0.0001 ) { Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE; return; } 		
		if ( ValeurDeBorneInf[Var] == ValeurDeBorneSup[Var] ) goto NextEdge;		
		ValeurDeVar = 0.0;				
    PNE_VariableProbingPreparerInstanciation( Pne, Var, ValeurDeVar );
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Var = Nv - Pivot;
		/* On ne doit pas avoir U[Var] = 0.0 */		
		if ( ValeurDeBorneSup[Var] < 0.9999 ) { Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE; return; } 		
		if ( ValeurDeBorneInf[Var] == ValeurDeBorneSup[Var] ) goto NextEdge;		
		ValeurDeVar = 1.0;						
    PNE_VariableProbingPreparerInstanciation( Pne, Var, ValeurDeVar );		
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
	}
	NextEdge: 
  Edge = Next[Edge];
}			   

return;
}

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingRazContraintesAAnalyser( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int Cnt; int NombreDeContraintes; char * ContrainteAAnalyser;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;
for ( Cnt = 0; Cnt < NombreDeContraintes ; Cnt++ ) {
  ContrainteAAnalyser[Cnt] = NON_PNE;
}
ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;
return;
}

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingRazCoupesDeProbingPotentielles( PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve )
{
int Cnt; int i; int * NumCntCoupesDeProbing; char * FlagCntCoupesDeProbing;
NumCntCoupesDeProbing = ProbingOuNodePresolve->NumCntCoupesDeProbing;
FlagCntCoupesDeProbing = ProbingOuNodePresolve->FlagCntCoupesDeProbing;
for ( i = 0; i < ProbingOuNodePresolve->NbCntCoupesDeProbing ; i++ ) {
  Cnt = NumCntCoupesDeProbing[i];
  FlagCntCoupesDeProbing[Cnt] = 0;
}
ProbingOuNodePresolve->NbCntCoupesDeProbing = 0;
return;
}

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingFixerUneVariableInstanciee( PROBLEME_PNE * Pne, PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve,
                                                    int Var, double ValeurDeVar )
{
int NombreDeVariables; int NombreDeContraintes; int Noeud; char BorneMiseAJour; char UneVariableAEteFixee;

if ( ValeurDeVar < 0 ) {
  printf("Bug dans PNE_VariableProbingFixerUneVariableInstanciee:\n");
	printf(" -> on cherche a fixer la variable binaire %d a %e\n",Var,ValeurDeVar);
	exit(0);
}

ProbingOuNodePresolve->Faisabilite = OUI_PNE;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
/* Il faut tout reinitialiser car les indicateurs de contrainte modifiee ou de variable modifiee
	 peuvent ne pas etre en accord */
memcpy( (char *) ProbingOuNodePresolve->BorneInfConnue, (char *) ProbingOuNodePresolve->BorneInfConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->BorneSupConnue, (char *) ProbingOuNodePresolve->BorneSupConnueSv, NombreDeVariables * sizeof( char ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneInf, (char *) ProbingOuNodePresolve->ValeurDeBorneInfSv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) ProbingOuNodePresolve->ValeurDeBorneSup, (char *) ProbingOuNodePresolve->ValeurDeBorneSupSv, NombreDeVariables * sizeof( double ) );

memcpy( (char *) ProbingOuNodePresolve->Bmin, (char *) ProbingOuNodePresolve->BminSv, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) ProbingOuNodePresolve->Bmax, (char *) ProbingOuNodePresolve->BmaxSv, NombreDeContraintes * sizeof( double ) );

ProbingOuNodePresolve->NbVariablesModifiees = 0;
ProbingOuNodePresolve->NbContraintesModifiees = 0;
memset( (char *) ProbingOuNodePresolve->VariableModifiee  , NON_PNE, NombreDeVariables   * sizeof( char ) );
memset( (int *) ProbingOuNodePresolve->NbFoisContrainteModifiee, 0, NombreDeContraintes * sizeof( int ) );

# if VERBOSE_VARIABLE_PROBING == 1
  printf("Instanciation de la variable %d a %e: Pas de solution\n",Var,ValeurDeVar);
# endif
		
/* On peut fixer definitivement la variable */
if ( Pne->ConflictGraph != NULL ) {
  Noeud = -1;
  if ( ValeurDeVar == Pne->UminTrav[Var] ) Noeud = Pne->ConflictGraph->Pivot + Var;
	else if ( ValeurDeVar == Pne->UmaxTrav[Var] ) Noeud = Var;
	else {
    printf("Erreur dans PNE_VariableProbingFixerUneVariableInstanciee la valeur d instanciation de la variable %d n'est ni le min ni le max\n",Var);
	}
	if ( Noeud < 0 ) return;

  /* Mise a jour des indicateurs de variables a instancier a nouveau */
  PNE_ProbingMajFlagVariablesAInstancier( Pne, Var, ValeurDeVar );
	
  PNE_ConflictGraphFixerLesNoeudsVoisinsDunNoeud( Pne, Noeud );
}
else {
  BorneMiseAJour = NON_PNE;
	if ( ValeurDeVar == Pne->UminTrav[Var] ) UneVariableAEteFixee = FIXE_AU_DEPART;
	else if ( ValeurDeVar == Pne->UmaxTrav[Var] ) UneVariableAEteFixee = FIXE_AU_DEPART; 
	else {
    printf("Erreur dans PNE_VariableProbingFixerUneVariableInstanciee la valeur d instanciation de la variable %d n'est ni le min ni le max\n",Var);
		return;
	}
  Pne->UminTrav[Var] = ValeurDeVar;		
  Pne->UmaxTrav[Var] = ValeurDeVar;		
  Pne->UTrav[Var] = ValeurDeVar;					
							
  PNE_ProbingMajBminBmax( Pne, Var, ValeurDeVar, BorneMiseAJour );
			
  PNE_MajIndicateursDeBornes( Pne, ProbingOuNodePresolve->ValeurDeBorneInf, ProbingOuNodePresolve->ValeurDeBorneSup,
	                            ProbingOuNodePresolve->BorneInfConnue, ProbingOuNodePresolve->BorneSupConnue,
															ValeurDeVar, Var, UneVariableAEteFixee, BorneMiseAJour );			
}

PNE_VariableProbingSauvegardesDonnees( Pne );

/* La liste des contraintes a analyser peut ne pas etre entierement a l'etat initial dans ce cas */
PNE_VariableProbingRazContraintesAAnalyser( Pne, ProbingOuNodePresolve );

/* La liste des coupes de probing potentielles peut ne pas etre entierement a l'etat initial dans ce cas */
PNE_VariableProbingRazCoupesDeProbingPotentielles( ProbingOuNodePresolve );

return;
}

/*----------------------------------------------------------------------------*/
/* On fait du variable probing en faisant du node presolve apres fixation
   de variables */
void PNE_VariableProbing( PROBLEME_PNE * Pne )
{
int Cnt; int Var; double ValeurDeVar; char CodeRet; int NombreDeVariablesTrav;
int NombreDeContraintesTrav; double * UminTrav; double * UminTravSv; double * UmaxTrav;
double * UmaxTravSv; double * UTrav; int NbFix; char * BorneInfConnue; int NbEdges;
char * BorneSupConnue; double * ValeurDeBorneInf; double * ValeurDeBorneSup;
int * NumeroDeCoupeDeProbing; int i; double * Umin0; double * Umax0; int NbVarAInstancier;
int * NumVarAInstancier; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; char FaireProbing;
char LaVariableDoitEtreFixee; double ValeurDeLaVariableAFixer; double DureeDuProbing;
int Iteration; int Nb; int NbTermesMatrice; int * NbTerm; double X; char * ContrainteAAnalyser;

Umin0 = NULL;
Umax0 = NULL;

FaireProbing = OUI_PNE;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

DureeDuProbing = DUREE_DU_PROBING;

if ( Pne->Controls != NULL ) {
  /* Pas de variable probing s'il s'agit d'un sous probleme appele par le solveur lui-meme */
  if ( Pne->Controls->FaireDuVariableProbing == NON_PNE ) {
    FaireProbing = NON_PNE;
	}
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("Probing on binaries and computing the conflict graph\n");
}

Pne->ArreterCliquesEtProbing = NON_PNE;
time( &(Pne->HeureDeCalendrierDebutCliquesEtProbing) );

NombreDeVariablesTrav = Pne->NombreDeVariablesTrav;
NombreDeContraintesTrav = Pne->NombreDeContraintesTrav;

UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
UTrav = Pne->UTrav;
NbTerm = Pne->NbTermTrav;

/* Sauvegarde des bornes inf et sup des variables. Elles vont nous servir a identifier les variables
   qu'on a pu fixer */
Umin0 = (double *) malloc( NombreDeVariablesTrav * sizeof( double ) );
if ( Umin0 == NULL ) goto Fin;
Umax0 = (double *) malloc( NombreDeVariablesTrav * sizeof( double ) );
if ( Umax0 == NULL ) goto Fin;
memcpy( (char *) Umin0, (char *) UminTrav, NombreDeVariablesTrav * sizeof( double ) );
memcpy( (char *) Umax0, (char *) UmaxTrav, NombreDeVariablesTrav * sizeof( double ) );

if ( Pne->ProbingOuNodePresolve == NULL ) {
  PNE_ProbingNodePresolveAlloc( Pne, &CodeRet );
  if ( CodeRet == NON_PNE ) goto Fin;;
}
ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

ProbingOuNodePresolve->NbParcours = 0;

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
  PNE_ProbingInitDetectionDesBornesVariables( Pne );
# endif

ProbingOuNodePresolve->VariableInstanciee = -1;

ProbingOuNodePresolve->NombreDeVariablesFixeesDansLeProbing = 0;

ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
NumeroDeCoupeDeProbing = ProbingOuNodePresolve->NumeroDeCoupeDeProbing;
NumVarAInstancier = ProbingOuNodePresolve->NumVarAInstancier;

# if PROBING_JUSTE_APRES_LE_PRESOLVE == OUI_PNE 
  PNE_ProbingInitVariablesAInstancierApresLePresolve( Pne, ProbingOuNodePresolve );
# else
  PNE_ProbingInitVariablesAInstancier( Pne, ProbingOuNodePresolve );
# endif
PNE_ProbingMajVariablesAInstancier( Pne, ProbingOuNodePresolve );

NbTermesMatrice = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
  NumeroDeCoupeDeProbing[Cnt] = -1;
	NbTermesMatrice += NbTerm[Cnt];
}
X =  NbTermesMatrice * NB_MAX_PARCOURS_MATRICE;
if ( X < 0 ) X = pow(2, ( 8 * sizeof( int ) ) - 1 ) - 1;

Nb = (int) floor( pow(2, ( 8 * sizeof( int ) ) - 1 ) - 1 );

if ( (int) X < Nb ) ProbingOuNodePresolve->SeuilNbTermesMatrice = (int) X;
else ProbingOuNodePresolve->SeuilNbTermesMatrice = Nb;

Iteration = 0;
DebutVariableProbing:

Iteration++;

ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;
for ( Cnt = 0; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
  ContrainteAAnalyser[Cnt] = NON_PNE;
}
ProbingOuNodePresolve->NombreDeContraintesAAnalyser = 0;

ProbingOuNodePresolve->Faisabilite = OUI_PNE;   

PNE_InitBorneInfBorneSupDesVariables( Pne );
PNE_CalculMinEtMaxDesContraintes( Pne, &(ProbingOuNodePresolve->Faisabilite) );
if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
  # if VERBOSE_VARIABLE_PROBING == 1
    printf("PNE_CalculMinEtMaxDesContraintes pas de solution\n");
	# endif
  goto Fin;
}
PNE_VariableProbingSauvegardesDonnees( Pne );

/* Liste des variables a tester */

NbVarAInstancier = ProbingOuNodePresolve->NbVarAInstancier;
if ( Pne->AffichageDesTraces == OUI_PNE && 0 ) {
  printf("Probing on %d variable(s)\n",ProbingOuNodePresolve->NbVarAInstancier);
}
for ( i = 0 ; i < NbVarAInstancier && FaireProbing == OUI_PNE ; i++ ) {
	
  if ( Pne->ConflictGraph != NULL ) { if ( Pne->ConflictGraph->NbEdges > 1000000 ) break; }

  if ( Pne->ArreterCliquesEtProbing == NON_PNE ) {
	
    time( &(Pne->HeureDeCalendrierCourantCliquesEtProbing) );
    Pne->TempsEcoule = difftime( Pne->HeureDeCalendrierCourantCliquesEtProbing, Pne->HeureDeCalendrierDebutCliquesEtProbing );
	  if ( Pne->TempsEcoule > DureeDuProbing ) {
		  Pne->ArreterCliquesEtProbing = OUI_PNE;
			if ( Pne->AffichageDesTraces == OUI_PNE ) {
			  printf("Probing was stopped. Timeout is %e elapsed is %e\n",DureeDuProbing,Pne->TempsEcoule);
			}
		}
  }
  if ( Pne->ArreterCliquesEtProbing == OUI_PNE ) break;	

  Var = NumVarAInstancier[i];
  # if DEBUG == OUI_PNE
    if ( Pne->TypeDeVariableTrav[Var] != ENTIER ) {
      printf("Bug dans PNE_VariableProbing la variable a instancier %d n'est pas binaire\n",Var);
			exit(0);
		}
  # endif
	
  LaVariableDoitEtreFixee = NON_PNE; 
	ValeurDeLaVariableAFixer = -1;
					 
	/* Instanciation a 1 */
  PNE_VariableProbingReinitDonnees( Pne );
	
	if ( BorneInfConnue[Var] == FIXE_AU_DEPART || BorneInfConnue[Var] == FIXATION_SUR_BORNE_INF ||
		   BorneInfConnue[Var] == FIXATION_SUR_BORNE_SUP ) continue;
  		
	ProbingOuNodePresolve->VariableInstanciee = Var;
  ProbingOuNodePresolve->Faisabilite = OUI_PNE;	
	ValeurDeVar = 1.0;
  ProbingOuNodePresolve->ValeurDeLaVariableInstanciee = ValeurDeVar;
  PNE_VariableProbingPreparerInstanciation( Pne, Var, ValeurDeVar );
	
  # if VERBOSE_VARIABLE_PROBING == 1
	  printf("Instanciation de la variable %d a %e\n",Var,ValeurDeVar);
	# endif
	
	NbEdges = 0;
	if ( Pne->ConflictGraph != NULL ) NbEdges = Pne->ConflictGraph->NbEdges; 
	
  # if VERBOSE_VARIABLE_PROBING == 1
    printf("Analyse de contraintes  NombreDeContraintesAAnalyser %d\n", ProbingOuNodePresolve->NombreDeContraintesAAnalyser);
	# endif
								
	PNE_AnalyseListeDeContraintes( Pne );				
		
	if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
    /* Pb infaisable si la variable vaut 1 => on la fixe a 0 */
    # if VERBOSE_VARIABLE_PROBING == 1
		  printf(" La valeur n'est pas admissible, la variable doit etre fixee a 0\n");
		# endif		
    LaVariableDoitEtreFixee = OUI_PNE;
	  ValeurDeLaVariableAFixer = 0;
		/* A ce stade la liste des variables et des contraintes modifiees est quand-meme correcte */
    /* La liste des contraintes a analyser peut ne pas etre entierement a l'etat initial dans ce cas */
    PNE_VariableProbingRazContraintesAAnalyser( Pne, ProbingOuNodePresolve );
    /* La liste des coupes de probing potentielles peut ne pas etre entierement a l'etat initial dans ce cas */
    PNE_VariableProbingRazCoupesDeProbingPotentielles( ProbingOuNodePresolve );		
	}
  else {
    PNE_CreerLesCoupesDeProbing( Pne, ProbingOuNodePresolve );  
	  if ( ProbingOuNodePresolve->NombreDeVariablesFixees != 0 ) PNE_MajConflictGraph( Pne, Var, ValeurDeVar );
	  /* Si des implications on ete crees (i.e. si le nombre de branches du graphe de conflit a ete augmente)
		   on met a jour la liste des variables qu'on testera au prochain coup */		
	  if ( Pne->ConflictGraph != NULL ) {
      if ( Pne->ConflictGraph->NbEdges > NbEdges && MAJ_FLAG == OUI_PNE ) {
        PNE_ProbingMajFlagVariablesAInstancier( Pne, Var, ValeurDeVar );			
			}
	  }
    # if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
      if ( Iteration == 1 ) PNE_ProbingConstruireContraintesDeBornes( Pne, ProbingOuNodePresolve, Umin0, Umax0 );
    # endif		
	}
	
	/* Instanciation a 0 */			
  PNE_VariableProbingReinitDonnees( Pne );	
	
	ProbingOuNodePresolve->VariableInstanciee = Var;
  ProbingOuNodePresolve->Faisabilite = OUI_PNE;						
	ValeurDeVar = 0.0;	
  ProbingOuNodePresolve->ValeurDeLaVariableInstanciee = ValeurDeVar;	
  PNE_VariableProbingPreparerInstanciation( Pne, Var, ValeurDeVar );
		
  # if VERBOSE_VARIABLE_PROBING == 1		
	  printf("Instanciation de la variable %d a %e\n",Var,ValeurDeVar);
	# endif
	
	NbEdges = 0;
	if ( Pne->ConflictGraph != NULL ) NbEdges = Pne->ConflictGraph->NbEdges;
	
  # if VERBOSE_VARIABLE_PROBING == 1
    printf("Analyse de contraintes  NombreDeContraintesAAnalyser %d\n", ProbingOuNodePresolve->NombreDeContraintesAAnalyser);
	# endif
			
  PNE_AnalyseListeDeContraintes( Pne );		
			
	if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) {	
	  if ( LaVariableDoitEtreFixee == OUI_PNE ) {
		  /* La valeur 1 n'etait deja pas admissible */
	    ProbingOuNodePresolve->Faisabilite = NON_PNE;
			goto Fin;
		}
    /* Pb infaisable si la variable vaut 0 => on la fixe a 1 */
    # if VERBOSE_VARIABLE_PROBING == 1
		  printf(" La valeur n'est pas admissible, la variable doit etre fixee a 1\n");
		# endif
    LaVariableDoitEtreFixee = OUI_PNE;
	  ValeurDeLaVariableAFixer = 1;
		/* A ce stade la liste des variables et des contraintes modifiees est quand-meme correcte */
    /* La liste des contraintes a analyser peut ne pas etre entierement a l'etat initial dans ce cas */
    PNE_VariableProbingRazContraintesAAnalyser( Pne, ProbingOuNodePresolve );
    /* La liste des coupes de probing potentielles peut ne pas etre entierement a l'etat initial dans ce cas */
    PNE_VariableProbingRazCoupesDeProbingPotentielles( ProbingOuNodePresolve );		
	}			
  else { 	
    PNE_CreerLesCoupesDeProbing( Pne, ProbingOuNodePresolve );  
	  if ( ProbingOuNodePresolve->NombreDeVariablesFixees != 0 ) PNE_MajConflictGraph( Pne, Var, ValeurDeVar );
	  /* Si des implications on ete crees (i.e. si le nombre de branches du graphe de conflit a ete augmente)
		   on met a jour la liste des variables qu'on testera au prochain coup */
	  if ( Pne->ConflictGraph != NULL ) {
      if ( Pne->ConflictGraph->NbEdges > NbEdges && MAJ_FLAG == OUI_PNE ) {
        PNE_ProbingMajFlagVariablesAInstancier( Pne, Var, ValeurDeVar );			
			}
	  }
    # if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
      if ( Iteration == 1 ) PNE_ProbingConstruireContraintesDeBornes( Pne, ProbingOuNodePresolve, Umin0, Umax0 );
    # endif				
	}

	/* Si la variable doit etre fixee on tient aussi compte du graphe de conflit */
  if ( LaVariableDoitEtreFixee == OUI_PNE ) {
    PNE_VariableProbingFixerUneVariableInstanciee( Pne, ProbingOuNodePresolve, Var, ValeurDeLaVariableAFixer );		
	  if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) goto Fin;			
	}
	
}

PNE_ProbingMajVariablesAInstancier( Pne, ProbingOuNodePresolve );
if ( ProbingOuNodePresolve->NbVarAInstancier > 0 ) {
  /*printf("1- NbVarAInstancier %d\n",ProbingOuNodePresolve->NbVarAInstancier);*/
  goto DebutVariableProbing;
}

if ( Pne->ConflictGraph != NULL ) {
   # if VERBOSE_VARIABLE_PROBING == 1
     printf("Extend conflict graph\n");
	 # endif	
	ProbingOuNodePresolve->Faisabilite = OUI_PNE;
  PNE_VariableProbingReinitDonnees( Pne );

  /* Si Cliques est plein on evite ExtendConflictGraph car c'est consommateur de temps de calcul */
	if ( Pne->Cliques == NULL ) PNE_ExtendConflictGraph( Pne );
	else if ( Pne->Cliques->Full != OUI_PNE ) PNE_ExtendConflictGraph( Pne );
 
  PNE_ConflictGraphFixerVariables( Pne );	
	if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) {
    /* Le probleme n'a pas de solution */
	 	goto Fin;
  }	
	
  PNE_ProbingMajVariablesAInstancier( Pne, ProbingOuNodePresolve );
  if ( ProbingOuNodePresolve->NbVarAInstancier > 0 ) {
    /*printf("2- NbVarAInstancier %d\n",ProbingOuNodePresolve->NbVarAInstancier);*/
    goto DebutVariableProbing;
  }
	
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( Pne->ConflictGraph != NULL ) {
    if ( Pne->ConflictGraph->NbEdges > 0 ) printf("Conflict graph has %d edges\n",Pne->ConflictGraph->NbEdges);
		else printf("Conflict graph is empty\n");
	}
	else printf("Conflict graph is empty\n");
}  

ProbingOuNodePresolve->Faisabilite = OUI_PNE;				

# if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
  PNE_ProbingCloseDetectionDesBornesVariables( Pne );
# endif

/* Mettre a jour les Umin max sv */

UminTravSv = Pne->UminTravSv;
UmaxTravSv = Pne->UmaxTravSv;

NbFix = 0;
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( Umin0[Var] == Umax0[Var] ) continue;
	if ( UminTrav[Var] == UmaxTrav[Var] ) {
	  NbFix++;
    # if PROBING_JUSTE_APRES_LE_PRESOLVE == OUI_PNE		  
			Pne->TypeDeBorneTrav[Var] = VARIABLE_FIXE;
		  Pne->TypeDeVariableTrav[Var] = REEL;			
		  UTrav[Var] = UminTrav[Var];
			UminTrav[Var] = Umin0[Var];
			UmaxTrav[Var] = Umax0[Var];
		# else
	    UminTravSv[Var] = UminTrav[Var];
	    UmaxTravSv[Var] = UminTrav[Var];		
    # endif		
	}
}
if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbFix > 0 ) printf("%d variable(s) fixed using probing techniques\n",NbFix);
  # if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
	  if ( Pne->ContraintesDeBorneVariable != NULL ) {
		  if ( Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne > 0 ) {
        printf("Adding %d implicit variable bound constraint(s)\n",Pne->ContraintesDeBorneVariable->NombreDeContraintesDeBorne);
			}
		}
  # endif		
}

ProbingOuNodePresolve->NombreDeVariablesFixeesDansLeProbing = NbFix;
  
ProbingOuNodePresolve->VariableInstanciee = -1;

/* En mode Probing apres le premier simplexe, il faut recalculer BmiSv BmasSv avec UminTravSv UmaxTravSv */

# if PROBING_JUSTE_APRES_LE_PRESOLVE == NON_PNE 
  if ( NbFix > 0 && 0 ) {
    /* En mode probing juste apres le presolve le recalcul est inutile */
	  /* En mode probing apres les simplexes du noeud racine le recalcul est inutile car:
		  - on met a jour UminSv et UmaxSv en fonction de Umin et Umax
			- le node presolve au noeud racine se calcule a chaque fois un Bmin Bmax fonction de Umin Umax
			  qui sont toujours egaux au Umin Umax au noeud racine
		*/
    PNE_InitBorneInfBorneSupDesVariables( Pne );
    PNE_CalculMinEtMaxDesContraintes( Pne, &(ProbingOuNodePresolve->Faisabilite) );
    /* Et on sauvegarde le resultat comme point de depart pour les noeuds suibvants */
    memcpy( (char *) ProbingOuNodePresolve->BminSv, (char *) ProbingOuNodePresolve->Bmin, Pne->NombreDeContraintesTrav * sizeof( double ) );
    memcpy( (char *) ProbingOuNodePresolve->BmaxSv, (char *) ProbingOuNodePresolve->Bmax, Pne->NombreDeContraintesTrav * sizeof( double ) );
  }
# endif

Fin:

free( Umin0 );
free( Umax0 );

if ( Pne->ProbingOuNodePresolve != NULL ) Pne->ProbingOuNodePresolve->VariableInstanciee = -1;

return;
}




