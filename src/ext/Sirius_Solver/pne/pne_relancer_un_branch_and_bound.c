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
# include "pne_fonctions.h"   
# include "pne_define.h"

# include "spx_define.h"
# include "spx_fonctions.h"  

# include "bb_define.h"
# include "bb_fonctions.h"

# include "prs_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 1

MATRICE_DE_CONTRAINTES * PNE_BranchAndBoundIntermediareMatriceDesContraintes( PROBLEME_PNE * , double );

/*----------------------------------------------------------------------------*/

MATRICE_DE_CONTRAINTES * PNE_BranchAndBoundIntermediareMatriceDesContraintes( PROBLEME_PNE * Pne, double MeilleurMinorant )
{
int NombreDeContraintesNatives; int NbTermesNecessaires; int * IndexDebut; int * NombreDeTermes;
int NombreDeVariablesNatives; int il; int NbTermesNatif; int NombreDeContraintes;
int NbContraintesNecessaires; double * SecondMembre; char * Sens;  int * Colonne; double * Coefficient;
MATRICE_DE_CONTRAINTES * Contraintes; int Nb; double * L; int Var; int ii; int iiMx;
char * ContrainteActivable; int NbT; int * Mdeb; int * NbTerm; char * SensContrainte; int * Nuvar;
double * B; double * A; int Cnt;

Contraintes = NULL;
/* Allocation */
Contraintes = (MATRICE_DE_CONTRAINTES *) malloc( sizeof( MATRICE_DE_CONTRAINTES ) );
if ( Contraintes == NULL ) return( NULL );

NombreDeContraintesNatives = Pne->NombreDeContraintesTrav;
NombreDeVariablesNatives = Pne->NombreDeVariablesTrav;
NbContraintesNecessaires = NombreDeContraintesNatives;

/* Pour les coupes */
NbContraintesNecessaires += Pne->Coupes.NombreDeContraintes;

/* Pour la borne sur le cout */
NbContraintesNecessaires++;

/* Pour la borne sur le cout de la solution entiere */
NbContraintesNecessaires++;

NbTermesNatif = Pne->TailleAlloueePourLaMatriceDesContraintes;

NbTermesNecessaires = NbTermesNatif;

/* Pour les coupes */
for ( Nb = 0 ; Nb < Pne->Coupes.NombreDeContraintes; Nb++ ) NbTermesNecessaires += Pne->Coupes.NbTerm[Nb];

/* Pour la borne sur le cout */
NbTermesNecessaires += NombreDeVariablesNatives;   

/* Pour la borne sur le cout de la solution entiere */
NbTermesNecessaires += NombreDeVariablesNatives;   

IndexDebut = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
NombreDeTermes = (int *) malloc( NbContraintesNecessaires * sizeof( int ) );
SecondMembre = (double *) malloc( NbContraintesNecessaires * sizeof( double ) );
Sens = (char *) malloc( NbContraintesNecessaires * sizeof( char ) );
Colonne = (int *) malloc( NbTermesNecessaires * sizeof( int ) );
Coefficient = (double *) malloc( NbTermesNecessaires * sizeof( double ) );
if ( IndexDebut == NULL || NombreDeTermes == NULL || SecondMembre == NULL || Sens == NULL || Colonne == NULL || Coefficient == NULL ) {
  free( IndexDebut ); free( NombreDeTermes ); free( SecondMembre ); free( Sens ); free( Colonne ); free( Coefficient );
	free( Contraintes );
	return( NULL );
}
Contraintes->IndexDebut = IndexDebut;
Contraintes->NombreDeTermes = NombreDeTermes;
Contraintes->SecondMembre = SecondMembre;
Contraintes->Sens = Sens;
Contraintes->Colonne = Colonne;
Contraintes->Coefficient = Coefficient;

/* Recopie de la matrice des contraintes natives mais uniquement celle qui sont activables */
ContrainteActivable = Pne->ContrainteActivable;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

il = 0;
NombreDeContraintes = 0;
for ( Cnt = 0 ; Cnt < NombreDeContraintesNatives ; Cnt++ ) {
  if ( ContrainteActivable[Cnt] == NON_PNE && 0 ) continue;
	IndexDebut[NombreDeContraintes] = il;
	NbT = 0;  
  ii = Mdeb[Cnt];
  iiMx = ii + NbTerm[Cnt];
	while ( ii < iiMx ) {
	  Colonne[il] = Nuvar[ii];
    Coefficient[il] = A[ii];
		ii++;
		il++;
		NbT++;
	}
	if ( NbT > 0 ) {
    NombreDeTermes[NombreDeContraintes] = NbT;
		Sens[NombreDeContraintes] = SensContrainte[Cnt];
		SecondMembre[NombreDeContraintes] = B[Cnt];
		NombreDeContraintes++;
	}
}
/*
printf("Pne->Coupes.NombreDeContraintes = %d\n",Pne->Coupes.NombreDeContraintes);

for ( Nb = 0 ; Nb < Pne->Coupes.NombreDeContraintes; Nb++ ) {
  IndexDebut[NombreDeContraintes] = il;
	NombreDeTermes[NombreDeContraintes] = Pne->Coupes.NbTerm[Nb];
  SecondMembre[NombreDeContraintes] = Pne->Coupes.B[Nb];
  Sens[NombreDeContraintes] = '<';
  ii = Pne->Coupes.Mdeb[Nb];
	iiMx = ii + Pne->Coupes.NbTerm[Nb];
	while ( ii < iiMx) {
	  Coefficient[il] = Pne->Coupes.A[ii];
		Colonne[il] = Pne->Coupes.Nuvar[ii];
		il++;
		ii++;
	}	
  NombreDeContraintes++;
}
*/
/* On se replace au noeud racine et on impose que le cout soit superieur ou egal au meilleur minorant */
/*
L = Pne->LTrav;
IndexDebut[NombreDeContraintes] = il;
Nb = 0;
for ( Var = 0 ; Var < NombreDeVariablesNatives ; Var++ ) {
  if ( L[Var] != 0.0 ) {
    Colonne[il] = Var;
    Coefficient[il] = L[Var];
		il++; Nb++;
	}
}
NombreDeTermes[NombreDeContraintes] = Nb;
SecondMembre[NombreDeContraintes] = MeilleurMinorant - 1.;
Sens[NombreDeContraintes] = '>';
NombreDeContraintes++;
*/
/* Pour la borne sur le cout de la solution entiere */
/*
L = Pne->LTrav;
IndexDebut[NombreDeContraintes] = il;
Nb = 0;
for ( Var = 0 ; Var < NombreDeVariablesNatives ; Var++ ) {
  if ( L[Var] != 0.0 ) {
    Colonne[il] = Var;
    Coefficient[il] = L[Var];
		il++; Nb++;
	}
}
NombreDeTermes[NombreDeContraintes] = Nb;
SecondMembre[NombreDeContraintes] = Pne->CoutOpt - Pne->Z0 + 1.;
Sens[NombreDeContraintes] = '<';
NombreDeContraintes++;
*/
Contraintes->NombreDeContraintes = NombreDeContraintes;

return( Contraintes );
}

/*----------------------------------------------------------------------------*/
void PNE_BranchAndBoundIntermediare( PROBLEME_PNE * Pne, double MeilleurMinorant )    
{
int Var; int NbMaxSol; CONTROLS Controls; PROBLEME_A_RESOUDRE Probleme; double S;
MATRICE_DE_CONTRAINTES * Contraintes; char BrnInfConnue; int * TypeDeBorne;
PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; int NombreDeVariables;
double * Xmin; double * Xmax; double * X; int * TypeDeVariable;  char * BorneSupConnue;
char * BorneInfConnue; double * ValeurDeBorneSup; double * ValeurDeBorneInf;
int * TypeDeVariablePne; 

if ( Pne->Controls != NULL ) return; /* C'est gere au niveau de l'appelant */
 
if ( Pne->AffichageDesTraces == OUI_PNE ) {
  printf("Trying to improve bounds restarting presolve\n");
}

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

NombreDeVariables = Pne->NombreDeVariablesTrav;

BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

TypeDeVariable = (int *) malloc( NombreDeVariables * sizeof( int ) );
memcpy( (char *) TypeDeVariable, (char *) Pne->TypeDeVariableTrav, NombreDeVariables * sizeof( int ) );

X = (double *) malloc( NombreDeVariables * sizeof( double ) );

Xmin = (double *) malloc( NombreDeVariables * sizeof( double ) );
memcpy( (char *) Xmin, (char *) ValeurDeBorneInf, NombreDeVariables * sizeof( double ) );

Xmax = (double *) malloc( NombreDeVariables * sizeof( double ) );
memcpy( (char *) Xmax, (char *) ValeurDeBorneSup, NombreDeVariables * sizeof( double ) );

TypeDeBorne = (int *) malloc( NombreDeVariables * sizeof( int ) );

for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {	
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
		X[Var] = ValeurDeBorneInf[Var]; 
    TypeDeBorne[Var] = VARIABLE_FIXE;
    /* A cause de tests dans init pne qui ne tolere pas qu'une variable entiere soit fixee et ait des bornes
       min et max identiques */		
    if ( TypeDeVariable[Var] == ENTIER ) TypeDeVariable[Var] = REEL;
    continue;
	}

  if ( BorneInfConnue[Var] != NON_PNE ) {
	  if ( BorneSupConnue[Var] != NON_PNE ) {
      TypeDeBorne[Var] = VARIABLE_BORNEE_DES_DEUX_COTES;
		}
		else {
      TypeDeBorne[Var] = VARIABLE_BORNEE_INFERIEUREMENT;
		}
	}
	else {
	  /* On ne traite pas le cas des variables bornees superieurement */
    TypeDeBorne[Var] = VARIABLE_NON_BORNEE;
		Xmin[Var] = -LINFINI_PNE;
		Xmax[Var] = LINFINI_PNE;
	}
}

MeilleurMinorant -= Pne->Z0;

Contraintes = PNE_BranchAndBoundIntermediareMatriceDesContraintes( Pne, MeilleurMinorant );
if ( Contraintes == NULL ) goto Fin;

Probleme.NombreDeVariables       = NombreDeVariables;
Probleme.TypeDeVariable          = TypeDeVariable;
Probleme.TypeDeBorneDeLaVariable = TypeDeBorne;
Probleme.X                       = X;
Probleme.Xmax                    = Xmax;
Probleme.Xmin                    = Xmin;
Probleme.CoutLineaire            = Pne->LTrav;
Probleme.NombreDeContraintes                   = Contraintes->NombreDeContraintes;
Probleme.SecondMembre                          = Contraintes->SecondMembre;
Probleme.Sens                                  = Contraintes->Sens;
Probleme.IndicesDebutDeLigne                   = Contraintes->IndexDebut;
Probleme.NombreDeTermesDesLignes               = Contraintes->NombreDeTermes;
Probleme.CoefficientsDeLaMatriceDesContraintes = Contraintes->Coefficient;
Probleme.IndicesColonnes                       = Contraintes->Colonne;
Probleme.VariablesDualesDesContraintes         = NULL;
Probleme.SortirLesDonneesDuProbleme = NON_PNE;
Probleme.AlgorithmeDeResolution     = SIMPLEXE; /* SIMPLEXE ou POINT_INTERIEUR */  
Probleme.CoupesLiftAndProject       = NON_PNE;

Probleme.AffichageDesTraces = OUI_PNE;

Probleme.FaireDuPresolve = OUI_PNE;          

Probleme.TempsDExecutionMaximum = 0;  

NbMaxSol = -1;
  
Probleme.NombreMaxDeSolutionsEntieres = NbMaxSol;

Probleme.ToleranceDOptimalite = 1.e-4; /* C'est en % donc 1.e-4 ca fait 1.e-6 */

Controls.Pne = Pne;
Controls.PneFils = NULL;
Controls.Presolve = NULL;   
Controls.PresolveUniquement = OUI_PNE;
Controls.FaireDuVariableProbing = OUI_PNE;
Controls.RechercherLesCliques = OUI_PNE;
    
PNE_SolveurProblemeReduit( &Probleme, &Controls );

if ( Controls.PresolveUniquement != OUI_PNE ) {
  S = Pne->Z0;
  for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
    S += X[Var] * Pne->LTrav[Var];
    if ( Pne->TypeDeVariableTrav[Var] == ENTIER ) {
	    if ( Pne->UTrav[Var] != Pne->UmaxTravSv[Var] && Pne->UTrav[Var] != Pne->UminTravSv[Var] ) {
        printf("Var entiere %d UTrav %e UminTravSv %e UmaxTravSv %e\n",Var,Pne->UTrav[Var],Pne->UminTravSv[Var],Pne->UmaxTravSv[Var]);
		  }
 	  }
  }
  printf("S %e\n",S);
  if ( Probleme.ExistenceDUneSolution == SOLUTION_OPTIMALE_TROUVEE ) printf("Solution optimale trouvee\n");
  else printf("Solution optimale pas trouvee ExistenceDUneSolution = %d\n",Probleme.ExistenceDUneSolution);

	exit(0);
	
}
else {
  /* On recupere les bornes */

  if ( Probleme.ExistenceDUneSolution != OUI_PNE ) {
	  printf("Pas de solution \n");
	}
		
	TypeDeVariablePne = Pne->TypeDeVariableTrav;

	int Stop;
	Stop = 0;
	
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
    if ( ValeurDeBorneInf[Var] == ValeurDeBorneSup[Var] ) continue;
    if ( TypeDeVariablePne[Var] == ENTIER ) {
      if ( ValeurDeBorneInf[Var] != ValeurDeBorneSup[Var] ) {
        if ( Xmin[Var] == Xmax[Var] ) {
          printf("Variable entiere %d fixee par le presolve a %e  ValeurDeBorneInf %e ValeurDeBorneSup %e\n",
					        Var,Xmin[Var],ValeurDeBorneInf[Var],ValeurDeBorneSup[Var]);
					ValeurDeBorneInf[Var] = Xmin[Var];
					ValeurDeBorneSup[Var] = Xmin[Var];
					BorneInfConnue[Var] = OUI_PNE;
					BorneSupConnue[Var] = OUI_PNE;
				}
			}
		}
		else {
      if ( Xmin[Var] > ValeurDeBorneInf[Var] + 1.e-5 ) {
        printf("Variable continue %d nouveau Xmin %e  ancien %e ecart %e\n",Var,Xmin[Var],ValeurDeBorneInf[Var],Xmin[Var]-ValeurDeBorneInf[Var]);
        ValeurDeBorneInf[Var] = Xmin[Var];
				BorneInfConnue[Var] = OUI_PNE;
				if ( Xmin[Var] > ValeurDeBorneSup[Var] ) {
				  printf("    BUG  Var %d Xmin %e ValeurDeBorneInf %e ValeurDeBorneSup %e\n",Var,Xmin[Var],ValeurDeBorneInf[Var],ValeurDeBorneSup[Var]);
					Stop = 1;
				}
			}		
      if ( Xmax[Var] < ValeurDeBorneSup[Var] - 1.e-5 ) {
        printf("Variable continue %d nouveau Xmax %e  ancien %e ecart %e\n",Var,Xmax[Var],ValeurDeBorneSup[Var],ValeurDeBorneSup[Var]-Xmax[Var]);
		    ValeurDeBorneSup[Var] = Xmax[Var];		
				BorneSupConnue[Var] = OUI_PNE;
				if ( Xmax[Var] < ValeurDeBorneInf[Var] ) {
				  printf("   BUG  Var %d Xmax %e ValeurDeBorneInf %e ValeurDeBorneSup %e\n",Var,Xmax[Var],ValeurDeBorneInf[Var],ValeurDeBorneSup[Var]);
					Stop = 1;
				}				
			}
		}
	}
	
  if ( Stop == 1 ) {
	  printf("Stop %d\n",Stop);
	  exit(0);
	}


	
}
Fin:

free( TypeDeVariable );
free( TypeDeBorne );
free( X );
free( Xmin );
free( Xmax );

return;
}
