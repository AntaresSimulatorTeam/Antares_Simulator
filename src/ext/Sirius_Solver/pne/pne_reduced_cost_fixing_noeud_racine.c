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

   FONCTION: Reduced cost fixing au noeud racine. Appele a chaque fois
	           qu'on trouve une nouvelle solution entiere pour fixer certaines
						 variables en fonction de la valeur de leur cout reduit au
						 noeud racine.
						 Remarque: si on fixe des variables on pourrait en profiter
						 pour refaire un probing avec determination de nouvelles
						 cliques.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# include "bb_define.h"
# include "bb_fonctions.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 0
# define TEST_RELANCE_BRANCH_AND_BOUND 0  

# define MARGE_CONTINUE 10
# define MARGE_DE_SECURITE 0.05

void PNE_ReducedCostFixingAuNoeudRacineRechercheNouvellesIncompatibilites( PROBLEME_PNE * , double , int * , int , double , int , int * ,
																															   				   int * , char * , double * , double * , double * , int * );
																																			
/*----------------------------------------------------------------------------*/

void PNE_ArchivagesPourReducedCostFixingAuNoeudRacine( PROBLEME_PNE * Pne,
																											 int * PositionDeLaVariable,
																											 double * CoutsReduits,
																											 double Critere )
{
int i ; double * CoutsReduitsAuNoeudRacine; int * PositionDeLaVariableAuNoeudRacine;
double MxCoutReduit; double * Umin; double * Umax; double X; int * TypeDeBorneTrav;

if ( Pne->CoutsReduitsAuNoeudRacine == NULL ) {
  Pne->CoutsReduitsAuNoeudRacine = (double *) malloc( Pne->NombreDeVariablesTrav * sizeof( double ) );
	if ( Pne->CoutsReduitsAuNoeudRacine == NULL ) return;
}

if ( Pne->PositionDeLaVariableAuNoeudRacine == NULL ) {
  Pne->PositionDeLaVariableAuNoeudRacine = (int *) malloc( Pne->NombreDeVariablesTrav * sizeof( int ) );
	if ( Pne->PositionDeLaVariableAuNoeudRacine == NULL ) {
	  free( Pne->CoutsReduitsAuNoeudRacine );
	  return;    
	}
}

CoutsReduitsAuNoeudRacine = Pne->CoutsReduitsAuNoeudRacine;
PositionDeLaVariableAuNoeudRacine = Pne->PositionDeLaVariableAuNoeudRacine;
 
Pne->CritereAuNoeudRacine = Critere;

MxCoutReduit = -1;
Umin = Pne->UminTrav;
Umax = Pne->UmaxTrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
for ( i = 0 ; i < Pne->NombreDeVariablesTrav ; i++ ) {
  PositionDeLaVariableAuNoeudRacine[i] = PositionDeLaVariable[i];	
  CoutsReduitsAuNoeudRacine[i] = CoutsReduits[i];
	X = fabs( CoutsReduits[i] * ( Umax[i] - Umin[i] ) );
  if ( TypeDeBorneTrav[i] != VARIABLE_BORNEE_DES_DEUX_COTES ) {	
    if ( X > MxCoutReduit ) {
      MxCoutReduit = X;
    }
	}
}

Pne->MxCoutReduitAuNoeudRacineFoisDeltaBornes = MxCoutReduit;

return;
}

/*----------------------------------------------------------------------------*/
/* Appele a chaque fois qu'on trouve une solution entiere */
void PNE_ReducedCostFixingAuNoeudRacine( PROBLEME_PNE * Pne )														
{
int Var; double Delta; double h; int * TypeDeBorne; int * TypeDeVariable; int Arret;
double * CoutsReduits; int * PositionDeLaVariable; int NbFix; int * T; int Nb;
int * Num; int j; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; int NombreDeVariables;
double NouvelleValeur; char * SensContrainte; double * B; char * ContrainteActivable;
int Cnt; int NbCntRelax; int NombreDeContraintes; int NbFixBin; int Faisabilite;
char * BorneSupConnue; double * ValeurDeBorneSup; char * BorneInfConnue; double * ValeurDeBorneInf;
char BrnInfConnue; char BorneMiseAJour; char UneVariableAEteFixee; double * Umin;
double * UminSv; double * Umax; double * UmaxSv; char * BminValide; char * BmaxValide;
double * BminSv; double * BmaxSv; double * Bmin; double * Bmax; char Mode;
int NombreDeVariablesNonFixes; int * NumeroDesVariablesNonFixes; int i; int NbBranchesAjoutees;
# if TEST_RELANCE_BRANCH_AND_BOUND == 1
  BB * Bb;
# endif

# if REDUCED_COST_FIXING_AU_NOEUD_RACINE == NON_PNE
  return;
# endif

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;
if ( ProbingOuNodePresolve == NULL ) return;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
NombreDeVariablesNonFixes = Pne->NombreDeVariablesNonFixes;
NumeroDesVariablesNonFixes = Pne->NumeroDesVariablesNonFixes;

T = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( T == NULL ) return;
Num = (int *) malloc( NombreDeVariables * sizeof( int ) );
if ( Num == NULL ) {
  free( T );
	return;
}
Nb = 0;
memset( (char *) T, 0, NombreDeVariables * sizeof( int ) );

TypeDeVariable = Pne->TypeDeVariableTrav;

/* Attention c'est le type de borne au noeud racine qu'il faut prendre */
memcpy( (char *) Pne->TypeDeBorneTrav, (char *) Pne->TypeDeBorneTravSv, NombreDeVariables * sizeof( int ) );
TypeDeBorne = Pne->TypeDeBorneTrav;

Umin = Pne->UminTrav;
UminSv = Pne->UminTravSv;
Umax = Pne->UmaxTrav;
UmaxSv = Pne->UmaxTravSv;

/* Attention c'est Umin au noeud racine qu'il faut prendre */
memcpy( (char *) Umin, (char *) UminSv, NombreDeVariables * sizeof( double ) );
/* Attention c'est Umax au noeud racine qu'il faut prendre */
memcpy( (char *) Umax, (char *) UmaxSv, NombreDeVariables * sizeof( double ) );

/* Initialisation des bornes inf et sup des variables */
PNE_InitBorneInfBorneSupDesVariables( Pne );

BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

CoutsReduits = Pne->CoutsReduitsAuNoeudRacine;
PositionDeLaVariable = Pne->PositionDeLaVariableAuNoeudRacine;

/* On recupere les bornes des contraintes au noeud racine */
Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;
BminSv = ProbingOuNodePresolve->BminSv;
BmaxSv = ProbingOuNodePresolve->BmaxSv;
memcpy( (char *) Bmin, (char *) BminSv, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Bmax, (char *) BmaxSv, NombreDeContraintes * sizeof( double ) );
BminValide = ProbingOuNodePresolve->BminValide;
BmaxValide = ProbingOuNodePresolve->BmaxValide;

SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
ContrainteActivable = Pne->ContrainteActivable;

Delta = Pne->CoutOpt - Pne->CritereAuNoeudRacine;
if ( Delta < 0. ) Delta = 0.; /* Car le cout entier est toujours superieur ou egal au cout relaxe */

Mode = PRESOLVE_SIMPLIFIE_POUR_REDUCED_COST_FIXING_AU_NOEUD_RACINE;

NbFix = 0;
NbFixBin = 0;
NbCntRelax = 0;
NbBranchesAjoutees = 0;

ProbingOuNodePresolve->Faisabilite = OUI_PNE;
ProbingOuNodePresolve->VariableInstanciee = -1;
ProbingOuNodePresolve->NbVariablesModifiees = 0;
ProbingOuNodePresolve->NbContraintesModifiees = 0;
Faisabilite = OUI_PNE;

for ( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];

  /*if ( TypeDeVariable[Var] != ENTIER ) continue;*/
	
  if ( fabs( CoutsReduits[Var] ) < ZERO_COUT_REDUIT ) continue;
	
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;
				 	  
	h = 0.0;
	Nb = 0;

  if ( TypeDeVariable[Var] == ENTIER ) {		
    if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {	
	    /* On regarde ce qu'il se passe si la variable passe a 0 */
		  Arret = NON_PNE;
			
      PNE_ReducedCostFixingConflictGraph( Pne, Var, ValeurDeBorneInf[Var], &h, Delta, CoutsReduits, ValeurDeBorneInf, ValeurDeBorneSup,
			                                    PositionDeLaVariable, &Nb, T, Num, &Arret );					
      if ( h > Delta ) {		
	      /* La variable entiere est fixee a Umax */				
        BorneMiseAJour = NON_PNE;
			  UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
				NouvelleValeur = ValeurDeBorneSup[Var];
        /* Applique le graphe de conflit s'il y a lieu, modifie Bmin et Bmax, fixe les variables entieres du graphe
				   si necessaire */
        PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
	
			  NbFix++;
				NbFixBin++;
      }
			else {
			  /* On regarde les incompatibilites avec les variables qui ne sont pas dans le graphe de conflit */
        PNE_ReducedCostFixingAuNoeudRacineRechercheNouvellesIncompatibilites( Pne, Delta, T, Var, ValeurDeBorneInf[Var], NombreDeVariables,
				                                                                      TypeDeVariable, PositionDeLaVariable, BorneInfConnue,
																																					    ValeurDeBorneInf,  ValeurDeBorneSup, CoutsReduits, &NbBranchesAjoutees ); 
			}
      for ( j = 0 ; j < Nb ; j++ ) T[Num[j]] = 0;			
    }  
    else if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {
	    /* On regarde ce qu'il se passe si la variable passe a 1 */
		  Arret = NON_PNE;
      PNE_ReducedCostFixingConflictGraph( Pne, Var, ValeurDeBorneSup[Var], &h, Delta, CoutsReduits, ValeurDeBorneInf, ValeurDeBorneSup,
			                                    PositionDeLaVariable, &Nb, T, Num, &Arret );				
      if ( h > Delta ) {		
	      /* La variable entiere sest fixee a Umin */
        BorneMiseAJour = NON_PNE;
			  UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
				NouvelleValeur = ValeurDeBorneInf[Var];
        /* Applique le graphe de conflit s'il y a lieu, modifie Bmin et Bmax, fixe les variables entieres du graphe
				   si necessaire */
        PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
				
			  NbFix++;
				NbFixBin++;		
      }
			else {
			  /* On regarde les incompatibilites avec les variables qui ne sont pas dans le graphe de conflit */
        PNE_ReducedCostFixingAuNoeudRacineRechercheNouvellesIncompatibilites( Pne, Delta, T, Var, ValeurDeBorneSup[Var], NombreDeVariables,
				                                                                      TypeDeVariable, PositionDeLaVariable, BorneInfConnue,
																																					    ValeurDeBorneInf,  ValeurDeBorneSup, CoutsReduits, &NbBranchesAjoutees ); 
			}
      for ( j = 0 ; j < Nb ; j++ ) T[Num[j]] = 0;			
    }
	}
	else	if ( TypeDeBorne[Var] == VARIABLE_BORNEE_DES_DEUX_COTES ) {	
    h = fabs( CoutsReduits[Var] * ( ValeurDeBorneSup[Var] - ValeurDeBorneInf[Var] ) );						
	  /* Variable continue */
    if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_INF ) {		
	    /* On regarde ce qu'il se passe si la variable passe a ValeurdeBorneSup */
      if ( h > Delta ) {			
		    NouvelleValeur = ( Delta / fabs( CoutsReduits[Var] ) ) + ValeurDeBorneInf[Var];				
			  if ( ValeurDeBorneSup[Var] - NouvelleValeur > MARGE_CONTINUE ) {
          /* On abaisse la borne sup */
          BorneMiseAJour = MODIF_BORNE_SUP;
			    UneVariableAEteFixee = NON_PNE;				
					NouvelleValeur = ( MARGE_DE_SECURITE * ValeurDeBorneSup[Var] ) + ( NouvelleValeur * ( 1 - MARGE_DE_SECURITE ) );
				  # if TRACES == 1
            printf("On peut abaisser la borne Sup de la variable %d : %e -> %e   abaissement %e  CoutsReduits %e\n",
						        Var,ValeurdeBorneSup[Var],NouvelleValeur,ValeurdeBorneSup[Var]-NouvelleValeur,CoutsReduits[Var]);
          # endif
          /* Applique le graphe de conflit s'il y a lieu, modifie Bmin et Bmax, fixe les variables entieres du graphe
				     si necessaire */
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
				
			    NbFix++;					
			  }			
      }
    }
    else if( PositionDeLaVariable[Var] == HORS_BASE_SUR_BORNE_SUP ) {				
	    /* On regarde ce qu'il se passe si la variable passe a Xmin */
      if ( h > Delta ) {				
		    NouvelleValeur = ValeurDeBorneSup[Var] - ( Delta / fabs( CoutsReduits[Var] ) );
			  if ( NouvelleValeur - ValeurDeBorneInf[Var] > MARGE_CONTINUE ) {					  
			    /* On remonte la borne inf */
          BorneMiseAJour = MODIF_BORNE_INF;
			    UneVariableAEteFixee = NON_PNE;			
				  NouvelleValeur = ( MARGE_DE_SECURITE * ValeurDeBorneInf[Var] ) + ( NouvelleValeur * ( 1 - MARGE_DE_SECURITE ) );
				  # if TRACES == 1
            printf("On peut relever la borne Inf de la variable %d : %e -> %e   relevement %e  Umax %e CoutsReduits %e\n",
						        Var,ValeurDeBorneInf[Var],NouvelleValeur,NouvelleValeur-ValeurDeBorneInf[Var],ValeurdeBorneSup[Var],CoutsReduits[Var]);				
          # endif
          /* Applique le graphe de conflit s'il y a lieu, modifie Bmin et Bmax, fixe les variables entieres du graphe
				     si necessaire */
          PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
				
			    NbFix++;																	  
        }
      }
	  }
	}  
}

free( T );
free( Num );
				
/* Pour l'instant on n'utilise pas Faisabilite */
PNE_PresolveSimplifie( Pne, ContrainteActivable, Mode, &Faisabilite );
if ( Faisabilite == NON_PNE ) {
  /*
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
	  printf("Alert: PNE_PresolveSimplifie report infeasability after reduced cost fixing. Problem may be infaisible\n");
	}
	*/
}

# if TEST_RELANCE_BRANCH_AND_BOUND == 1
  /* On regarde si on a pu fixer des variables ou abaisser des bornes afin de lancer un presolve plus
     complet si c'est prometteur */
  NbFix = 0;
  NbFixBin = 0;
  for ( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {
    Var = NumeroDesVariablesNonFixes[i];
    if ( BorneInfConnue[Var] == FIXE_AU_DEPART ) continue;		
    if ( TypeDeVariable[Var] == ENTIER ) {
	    if ( ValeurDeBorneInf[Var] > Umin[Var] + 1.e-6 ) { NbFix++; NbFixBin++; }
	    if ( ValeurDeBorneSup[Var] < Umax[Var] - 1.e-6 ) { NbFix++; NbFixBin++; }
	  }
    else {		
	    if ( ValeurDeBorneInf[Var] > Umin[Var] + 1.e-6 ) NbFix++; 	
		  if ( ValeurDeBorneSup[Var] < Umax[Var] - 1.e-6 ) NbFix++;
	  }  
	  if ( NbFixBin > 0 || NbFix > 10 ) {
      Bb = (BB *) Pne->ProblemeBbDuSolveur;
      if ( Pne->Controls == NULL ) {	
		    PNE_BranchAndBoundIntermediare( Pne, Bb->ValeurDuMeilleurMinorant );
				break;
		  }
    }  
	}
# endif

/* Synthese */
/* On recupere Umin, Umax */

NbFix = 0;
NbFixBin = 0;

for ( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];

  if ( BorneInfConnue[Var] == FIXE_AU_DEPART ) continue;
  if ( TypeDeVariable[Var] == ENTIER ) {
	  if ( ValeurDeBorneInf[Var] > Umin[Var] + 1.e-6 ) { NbFix++; NbFixBin++; }
	  Umin[Var] = ValeurDeBorneInf[Var];
	  UminSv[Var] = ValeurDeBorneInf[Var];
	  if ( ValeurDeBorneSup[Var] < Umax[Var] - 1.e-6 ) { NbFix++; NbFixBin++; }
	  Umax[Var] = ValeurDeBorneSup[Var];
	  UmaxSv[Var] = ValeurDeBorneSup[Var];
	}
  else {		
	  if ( ValeurDeBorneInf[Var] > Umin[Var] + 1.e-6 ) NbFix++; 	
		if ( ValeurDeBorneSup[Var] < Umax[Var] - 1.e-6 ) NbFix++;
		
	  Umax[Var] = ValeurDeBorneSup[Var];
	  UmaxSv[Var] = ValeurDeBorneSup[Var];
	}
}

/* On recalcule Bmin Bmax */
PNE_InitBorneInfBorneSupDesVariables( Pne );
PNE_CalculMinEtMaxDesContraintes( Pne, &Faisabilite );
/* Et on sauvegarde le resultat comme point de depart pour les noeuds suivants */
memcpy( (char *) BminSv, (char *) Bmin, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) BmaxSv, (char *) Bmax, NombreDeContraintes * sizeof( double ) );	

/* Contraintes inactives */

NbCntRelax = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( ContrainteActivable[Cnt] == NON_PNE ) NbCntRelax++;
	else {
	  if ( SensContrainte[Cnt] == '<' ) {
		  if ( BmaxValide[Cnt] == OUI_PNE ) {
        if ( Bmax[Cnt] < B[Cnt] - 1.e-6 ) {
				  ContrainteActivable[Cnt] = NON_PNE;
				  NbCntRelax++;
				}
			}
		}   
	}
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbFix != 0 || NbCntRelax != 0 || NbBranchesAjoutees != 0 ) {
	  printf("Reduced cost fixing at root node: \n");
    if ( NbFix != 0 ) printf("        Bounds tightened: %d of which %d binarie(s)\n",NbFix,NbFixBin);
  	if ( NbBranchesAjoutees != 0 ) printf("        %d implications added. Conflict graph has %d edges\n",NbBranchesAjoutees,Pne->ConflictGraph->NbEdges);
  	if ( NbCntRelax != 0 ) printf("        Useless constraints: %d over %d\n",NbCntRelax,NombreDeContraintes);
	}
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_ReducedCostFixingAuNoeudRacineRechercheNouvellesIncompatibilites( PROBLEME_PNE * Pne, double Delta, int * T,
                                                                           int VarEtudiee, double ValeurDeVariableEtudiee,
																																					 int NombreDeVariables, int * TypeDeVariable,
																																					 int * PositionDeLaVariable, char * BorneInfConnue,
																																					 double * ValeurDeBorneInf, double * ValeurDeBorneSup,
																																					 double * CoutsReduits, int * NbEdges )
{
int Var; char  BrnInfConnue; int PosVariable; double DeltaCout; double h;

int * Cdeb; int * Csui; int * NumContrainte; int * Mdeb; int * NbTerm; int * Nuvar; int ic; int il; int ilMax; int Cnt; char OnPrend;

return;  /* Ca marche mais ca sert a rien et ca consomme du temps */

DeltaCout = fabs( CoutsReduits[VarEtudiee] * ( ValeurDeBorneSup[VarEtudiee] - ValeurDeBorneInf[VarEtudiee] ) );

h = fabs( Pne->CoutOpt ) * 0.01;
if ( h < 1 ) h = 1;
if ( h > 1000 ) h = 1000;

Delta += h;

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;

for ( Var = VarEtudiee + 1 ; Var < NombreDeVariables ; Var++ ) {

  if ( TypeDeVariable[Var] != ENTIER ) continue;
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;	
	if ( T[Var] == 1 ) continue; 
	PosVariable = PositionDeLaVariable[Var];
  if( PosVariable == HORS_BASE_SUR_BORNE_INF || PosVariable == HORS_BASE_SUR_BORNE_SUP ) {
	  /* On change la variable de borne */
    h = DeltaCout + fabs( CoutsReduits[Var] * ( ValeurDeBorneSup[Var] - ValeurDeBorneInf[Var] ) );  
    if ( h > Delta ) {

      /* Si on trouve une contrainte native avec les 2 variables, seulles on ne cree pas l'implication */
			OnPrend = OUI_PNE;
      ic = Cdeb[VarEtudiee];
			while ( ic >= 0 && OnPrend == OUI_PNE ) {
        Cnt = NumContrainte[ic];
				if ( NbTerm[Cnt] == 2 ) {
				  il = Mdeb[Cnt];
				  ilMax = il + NbTerm[Cnt];
				  while ( il < ilMax ) {
            if ( Nuvar[il] == Var ) {
						  OnPrend = NON_PNE;
							break;
						}
				    il++;
				  }
			  }
			  ic = Csui[ic];
			}
			if ( OnPrend == NON_PNE ) continue;
		
			if( PosVariable == HORS_BASE_SUR_BORNE_INF ) {
			  /* On ne peut pas la passer a 1 */
			  BorneInfConnue[Var] = FIXATION_SUR_BORNE_INF;
			}
			else {
			  /* On ne peut pas la passer a 0 */
			  BorneInfConnue[Var] = FIXATION_SUR_BORNE_SUP;
			}

	    Pne->ProbingOuNodePresolve->NumeroDesVariablesFixees[0] = Var;
      Pne->ProbingOuNodePresolve->NombreDeVariablesFixees = 1;			
			
      PNE_MajConflictGraph( Pne, VarEtudiee, ValeurDeVariableEtudiee );

			/* On recupere l'information sur la borne inf */
			*NbEdges = *NbEdges + 1;
			BorneInfConnue[Var] = BrnInfConnue;
      Pne->ProbingOuNodePresolve->NombreDeVariablesFixees = 0;
			
		}
  }
}

return;
}
