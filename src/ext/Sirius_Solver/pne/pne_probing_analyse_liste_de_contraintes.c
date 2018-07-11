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

   FONCTION: Variable probing et node presolve
                
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

# define MAJ_BMIN 1
# define MAJ_BMAX 2
# define FORCING_BMIN 1
# define FORCING_BMAX 2
# define MAX_TERMES_CONTRAINTES_A_ANALYSER 10
# define SEUIL_NOMBRE_DE_CONTRAINTES_POUR_MAX_TERMES 100000

void PNE_ProbingControleFaisabiliteSiMajBminOuBmax( PROBLEME_PNE * , char , char , double , char , double , char , double , int );
void ControleBmin( PROBLEME_PNE * , int , double );
void ControleBmax( PROBLEME_PNE * , int , double );
																										
/*----------------------------------------------------------------------------------------------------*/

void PNE_ProbingControleFaisabiliteSiMajBminOuBmax( PROBLEME_PNE * Pne, char SensContrainte,
                                                    char BorneMiseAJour, double B,
                                                    char BminValide, double Bmin,
                                                    char BmaxValide, double Bmax, int Cnt )
{
if ( BminValide == OUI_PNE && BmaxValide == OUI_PNE ) {
  if ( Bmin > Bmax + SEUIL_DADMISSIBILITE ) {
    Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE;
		return;
	}  
}

if ( BorneMiseAJour == MAJ_BMIN ) {
  if ( Bmin > B + SEUIL_DADMISSIBILITE ) {	
    Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE;			 
    return;
  }
}
else if ( BorneMiseAJour == MAJ_BMAX ) {
  if ( SensContrainte == '=' ) {
    if ( Bmax < B - SEUIL_DADMISSIBILITE ) {		
			Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE;			
			return;							
		}		
	}
	else { /* Contrainte < */
    if ( Bmax < B - SEUIL_DADMISSIBILITE ) {		
		  if ( Pne->ProbingOuNodePresolve->VariableInstanciee < 0 ) return;		
		  /* On va essayer une coupe de probing sur la contrainte */
      if ( Pne->ProbingOuNodePresolve->FlagCntCoupesDeProbing[Cnt] == 0 ) {
        Pne->ProbingOuNodePresolve->NumCntCoupesDeProbing[Pne->ProbingOuNodePresolve->NbCntCoupesDeProbing] = Cnt;
				Pne->ProbingOuNodePresolve->NbCntCoupesDeProbing++;
        Pne->ProbingOuNodePresolve->FlagCntCoupesDeProbing[Cnt] = 1;
			}			
		}	
	}
}
return;
}

/*----------------------------------------------------------------------------------------------------*/
void ControleBmin( PROBLEME_PNE * Pne, int Cnt, double Bmin )
{ int il; int ilMax; double S; double * A; int * Mdeb; int * NbTerm; int * Nuvar; int Var; char BrnInfConnue;
char * BorneSupConnue; char * BorneInfConnue; double * ValeurDeBorneSup; double * ValeurDeBorneInf;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;
BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;
ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
S = 0;
il = Mdeb[Cnt];
ilMax =il + NbTerm[Cnt];
while ( il < ilMax) {
  Var = Nuvar[il];		
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
	  S += A[il] * ValeurDeBorneInf[Var];
	}
	else {
	  if ( A[il] > 0.0 ) S += A[il] * ValeurDeBorneInf[Var];				       								
	  else S += A[il] * ValeurDeBorneSup[Var];			
	}
  il++;
}
if ( fabs( S - Bmin ) > 1.e-8 && 0 ) {
  printf("S %e Bmin %e Cnt %d  NbFoisContrainteModifiee %d\n",S,Bmin,Cnt,Pne->ProbingOuNodePresolve->NbFoisContrainteModifiee[Cnt]);
  Pne->ProbingOuNodePresolve->Bmin[Cnt] = S;	
	/*exit(0);*/
}
return;
}
/*----------------------------------------------------------------------------------------------------*/
void ControleBmax( PROBLEME_PNE * Pne, int Cnt, double Bmax )
{ int il; int ilMax; double S; double * A; int * Mdeb; int * NbTerm; int * Nuvar; int Var; char BrnInfConnue;
char * BorneSupConnue; char * BorneInfConnue; double * ValeurDeBorneSup; double * ValeurDeBorneInf;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
A = Pne->ATrav;
Nuvar = Pne->NuvarTrav;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;
BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;
ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
S = 0;
il = Mdeb[Cnt];
ilMax =il + NbTerm[Cnt];
while ( il < ilMax) {
  Var = Nuvar[il];		
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
	  S += A[il] * ValeurDeBorneInf[Var];
	}
	else {
	  if ( A[il] > 0.0 ) S += A[il] * ValeurDeBorneSup[Var];				       								
	  else S += A[il] * ValeurDeBorneInf[Var];			
	}
  il++;
}
if ( fabs( S - Bmax ) > 1.e-8 && 0 ) {
  printf("S %e Bmax %e Cnt %d  NbFoisContrainteModifiee %d\n",S,Bmax,Cnt,Pne->ProbingOuNodePresolve->NbFoisContrainteModifiee[Cnt]);
  Pne->ProbingOuNodePresolve->Bmax[Cnt] = S;	
	/*exit(0);*/
}
return;
}

/*----------------------------------------------------------------------------------------------------*/

void PNE_ProbingMajBminBmax( PROBLEME_PNE * Pne, int Var, double ValeurDeVar, char BorneMiseAJour )
{
int ic; int * Cdeb; int * Csui; int * NumContrainte; int Cnt; double * A; long double Ai; 
char * BminValide; char * BmaxValide; double * Bmin; double * Bmax; long double ValeurDeBorneInf;
long double ValeurDeBorneSup; char * SensContrainte; long double X; long double ValDeVar;
double * B; int * NumeroDeContrainteModifiee; int * NbFoisContrainteModifiee;
char MajCnt; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; 

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
SensContrainte = Pne->SensContrainteTrav;   
B = Pne->BTrav;
ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;
BminValide = ProbingOuNodePresolve->BminValide;
BmaxValide = ProbingOuNodePresolve->BmaxValide;
Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;
ValeurDeBorneInf = (long double) ProbingOuNodePresolve->ValeurDeBorneInf[Var];
ValeurDeBorneSup = (long double) ProbingOuNodePresolve->ValeurDeBorneSup[Var];
NumeroDeContrainteModifiee = ProbingOuNodePresolve->NumeroDeContrainteModifiee;
NbFoisContrainteModifiee = ProbingOuNodePresolve->NbFoisContrainteModifiee;
ValDeVar = (long double) ValeurDeVar;

ic = Cdeb[Var];
while ( ic >= 0 ) {
  Ai = (long double) A[ic];	
  Cnt = NumContrainte[ic];
  if ( BminValide[Cnt] == OUI_PNE ) {	
	  MajCnt = NON_PNE;
    if ( BorneMiseAJour == MODIF_BORNE_INF ) {
		  /* C'est une modification de borne inf : peut etre concentre en 1 seul test */
			if ( Ai > 0 ) {
				/*ControleBmin( Pne, Cnt, Bmin[Cnt] );*/        
        X = (long double) Bmin[Cnt];
				X += Ai * (-ValeurDeBorneInf +ValDeVar);
			  Bmin[Cnt] = (double) X;				
	      MajCnt = OUI_PNE;				
			}
		}
		else  if ( BorneMiseAJour == MODIF_BORNE_SUP ) {
		  /* C'est une modification de borne sup */
			if ( Ai < 0 ) {			
				/*ControleBmin( Pne, Cnt, Bmin[Cnt] );*/ 
        X = (long double) Bmin[Cnt];
				X += Ai * (-ValeurDeBorneSup +ValDeVar);
			  Bmin[Cnt] = (double) X;
	      MajCnt = OUI_PNE;				
			}
		}
    else {
			/*ControleBmin( Pne, Cnt, Bmin[Cnt] );*/
      X = (long double) Bmin[Cnt];
	    if ( Ai >= 0.0 ) X += Ai * (-ValeurDeBorneInf +ValDeVar);			
			else X += Ai * (-ValeurDeBorneSup +ValDeVar);
			Bmin[Cnt] = (double) X;
	    MajCnt = OUI_PNE;			
		}
	  if ( MajCnt == OUI_PNE ) {
      if ( NbFoisContrainteModifiee[Cnt] == 0 ) {
        NumeroDeContrainteModifiee[ProbingOuNodePresolve->NbContraintesModifiees] = Cnt;
	      ProbingOuNodePresolve->NbContraintesModifiees++;
      }					
     	NbFoisContrainteModifiee[Cnt]++;
      PNE_ProbingControleFaisabiliteSiMajBminOuBmax( Pne, SensContrainte[Cnt], MAJ_BMIN, B[Cnt], BminValide[Cnt],
			                                               Bmin[Cnt], BmaxValide[Cnt], Bmax[Cnt], Cnt );
	    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;			 																													 
		}
	}	
  if ( BmaxValide[Cnt] == OUI_PNE ) {
	  MajCnt = NON_PNE;
    if ( BorneMiseAJour == MODIF_BORNE_INF ) {
		  /* C'est une modification de borne inf */
			if ( Ai < 0 ) {
				/*ControleBmax( Pne, Cnt, Bmax[Cnt] );*/ 
        X = (long double) Bmax[Cnt];
				X += Ai * (-ValeurDeBorneInf +ValDeVar);
			  Bmax[Cnt] = (double) X;				
	      MajCnt = OUI_PNE;								
			}
		}
		else  if ( BorneMiseAJour == MODIF_BORNE_SUP ) {  
		  /* C'est une modification de borne sup */
			if ( Ai > 0 ) {
				/*ControleBmax( Pne, Cnt, Bmax[Cnt] );*/
        X = (long double) Bmax[Cnt];
				X += Ai * (-ValeurDeBorneSup +ValDeVar);
			  Bmax[Cnt] = (double) X;				
	      MajCnt = OUI_PNE;				
			}
		}
    else {
			/*ControleBmax( Pne, Cnt, Bmax[Cnt] );*/
      X = (long double) Bmax[Cnt];
	    if ( Ai >= 0.0 ) X += Ai * (-ValeurDeBorneSup +ValDeVar);			
			else X += Ai * (-ValeurDeBorneInf +ValDeVar);
			Bmax[Cnt] = (double) X;
	    MajCnt = OUI_PNE;			
		}
	  if ( MajCnt == OUI_PNE ) {
      if ( NbFoisContrainteModifiee[Cnt] == 0 ) {
        NumeroDeContrainteModifiee[ProbingOuNodePresolve->NbContraintesModifiees] = Cnt;
	      ProbingOuNodePresolve->NbContraintesModifiees++;
      }				
     	NbFoisContrainteModifiee[Cnt]++;
      PNE_ProbingControleFaisabiliteSiMajBminOuBmax( Pne, SensContrainte[Cnt], MAJ_BMAX, B[Cnt], BminValide[Cnt],
			                                               Bmin[Cnt], BmaxValide[Cnt], Bmax[Cnt], Cnt );
	    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;			 			
		}
	}
	ic = Csui[ic];
}

return;
}
   
/*----------------------------------------------------------------------------*/
/* Si on fixe la variable Var a la valeur ValeurDeVar, on met a jour la liste
   des contraintes a analyser (celles qui sont modifiees), les seconds membres
	 et leurs plages de variation */
void PNE_InitListeDesContraintesAExaminer( PROBLEME_PNE * Pne, int Var, double ValeurDeVar,
                                           char BorneMiseAJour )
{
int ic; int * Cdeb; int * Csui; int * NumContrainte; int Cnt; int NombreDeContraintesAAnalyser;
int * NumeroDeContrainteAAnalyser; char SensDeVariation; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve;
char * SensContrainte; double * A; char * BmaxValide; double * Bmax; double * B; char * ContrainteAAnalyser;
int NombreDeContraintes; int IndexLibreContraintesAAnalyser; int * NbTerm; 

PNE_ProbingMajBminBmax( Pne, Var, ValeurDeVar, BorneMiseAJour );
if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;

if ( Pne->ProbingOuNodePresolve->VariableInstanciee < 0 ) return;

Cdeb = Pne->CdebTrav;     
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

NombreDeContraintesAAnalyser = ProbingOuNodePresolve->NombreDeContraintesAAnalyser;
NumeroDeContrainteAAnalyser = ProbingOuNodePresolve->NumeroDeContrainteAAnalyser;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;
IndexLibreContraintesAAnalyser = ProbingOuNodePresolve->IndexLibreContraintesAAnalyser;

/* ValeurDeVar est le nouvelle valeur de la variable */
SensDeVariation = '+'; /* On initialise le sens a "la variable augmente" */
if ( ValeurDeVar < ProbingOuNodePresolve->ValeurDeBorneSup[Var] ) {
  /* La variable decroit */
	SensDeVariation = '-';
}

NombreDeContraintes = Pne->NombreDeContraintesTrav;
NbTerm = Pne->NbTermTrav;
SensContrainte = Pne->SensContrainteTrav;
A = Pne->ATrav;

BmaxValide = ProbingOuNodePresolve->BmaxValide;
Bmax = ProbingOuNodePresolve->Bmax;
B = Pne->BTrav;

ic = Cdeb[Var];
while ( ic >= 0 ) {
  Cnt = NumContrainte[ic];
	if ( NombreDeContraintes > SEUIL_NOMBRE_DE_CONTRAINTES_POUR_MAX_TERMES ) {
    if ( NbTerm[Cnt] > MAX_TERMES_CONTRAINTES_A_ANALYSER ) goto NextIc;
	}
	if ( SensContrainte[Cnt] == '<' ) {
	  if ( BmaxValide[Cnt] == OUI_PNE ) {
      if ( Bmax[Cnt] <= B[Cnt] + SEUIL_DADMISSIBILITE ) goto NextIc;
    }				
    if ( A[ic] >= 0 ) {
		  if ( SensDeVariation == '-' ) goto NextIc;
		}					
    else {
		  /* A est negatif */
		  if ( SensDeVariation == '+' ) goto NextIc;
	  }
	}
	
	if ( ContrainteAAnalyser[Cnt] == NON_PNE ) {
    if ( NombreDeContraintesAAnalyser < NombreDeContraintes ) {	
      /* On l'ajoute a la liste */
	    NumeroDeContrainteAAnalyser[NombreDeContraintesAAnalyser] = Cnt;			
	    NombreDeContraintesAAnalyser++;
		}
		else {
		  /* Pour eviter les debordements de tables */
			if ( IndexLibreContraintesAAnalyser < NombreDeContraintes ) {
	      NumeroDeContrainteAAnalyser[IndexLibreContraintesAAnalyser] = Cnt;			
        IndexLibreContraintesAAnalyser++;
			}
		}
		ContrainteAAnalyser[Cnt] = OUI_PNE;
	}
	
	NextIc:
  ic = Csui[ic];
}

ProbingOuNodePresolve->NombreDeContraintesAAnalyser = NombreDeContraintesAAnalyser;
ProbingOuNodePresolve->IndexLibreContraintesAAnalyser = IndexLibreContraintesAAnalyser;

return;
}

/*----------------------------------------------------------------------------*/

void PNE_VariableProbingAppliquerLeConflictGraph( PROBLEME_PNE * Pne, int Var, double ValeurDeVar,
                                                  char BorneMiseAJour, char UneVariableAEteFixee )
{
int Edge; int Noeud; int Complement; int Nv; int Pivot; double * ValeurDeBorneSup; double * ValeurDeBorneInf;   
char * BorneInfConnue; char * BorneSupConnue; int * First; int * Adjacent; int * Next;

ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = Pne->ProbingOuNodePresolve->BorneSupConnue;

PNE_InitListeDesContraintesAExaminer( Pne, Var, ValeurDeVar, BorneMiseAJour );
if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;

PNE_MajIndicateursDeBornes( Pne, ValeurDeBorneInf, ValeurDeBorneSup, BorneInfConnue, BorneSupConnue,
                            ValeurDeVar, Var, UneVariableAEteFixee, BorneMiseAJour );			

if ( Pne->TypeDeVariableTrav[Var] != ENTIER ) return;

if ( Pne->ConflictGraph == NULL ) return;
Pivot = Pne->ConflictGraph->Pivot;

BorneMiseAJour = NON_PNE;

if ( ValeurDeVar == 1.0 ) { Noeud = Var; Complement = Pivot + Var; }
else { Noeud = Pivot + Var; Complement = Var; }

First = Pne->ConflictGraph->First;
Adjacent = Pne->ConflictGraph->Adjacent;
Next = Pne->ConflictGraph->Next;

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
    /* On place la variable dans la liste des implications */
		/*  */
		ValeurDeVar = 0.0;
    UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;
		/*  */
    PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, ValeurDeVar, BorneMiseAJour, UneVariableAEteFixee );
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
	}
  else {
    /* La valeur borne inf est interdite pour la variable */
		/* On doit donc fixer la variable a Umax et fixer les voisins de ce noeud */
	  Var = Nv - Pivot;
		/* On ne doit pas avoir U[Var] = 0.0 */
		if ( ValeurDeBorneSup[Var] < 0.9999 ) { Pne->ProbingOuNodePresolve->Faisabilite = NON_PNE; return; }				
		if ( ValeurDeBorneInf[Var] == ValeurDeBorneSup[Var] ) goto NextEdge;
    /* On place la variable dans la liste des implications */
		/*  */		
		ValeurDeVar = 1.0;
    UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;
		/*  */		
    PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, ValeurDeVar, BorneMiseAJour, UneVariableAEteFixee );		
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
	}
	NextEdge:
  Edge = Next[Edge];
}			

return;
}

/*----------------------------------------------------------------------------*/

void PNE_AnalyseListeDeContraintes( PROBLEME_PNE * Pne )
{
int i; int Cnt; char XsValide; char XiValide; double Ai; 
int il; int ilMax;int Var; char BorneMiseAJour; char UneVariableAEteFixee;  
int * NumeroDeContrainteAAnalyser; int * Mdeb; int * NbTerm; double * A; int * Nuvar;
double Xi; double Xs; int NombreDeContraintesAAnalyser; int * TypeDeVariable;  char * BorneInfConnue;
char * BorneSupConnue; char BrnInfConnue; double * ValeurDeBorneSup; double * ValeurDeBorneInf;
double BminNew; double BmaxNew; double S; char * SensContrainte; double NouvelleValeur;
char * BminValide; char * BmaxValide; double * Bmin; double * Bmax; double * B; int Nb;
char Flag; long double Xi0; long double Xs0; int NbParcours; int SeuilNbTermesMatrice;
char SensCnt; char BmnValide; char BmxValide; double Bmn; double Bmx; double BCnt; char ForcingContraint;
PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve; char * ContrainteAAnalyser;

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;

NbParcours = ProbingOuNodePresolve->NbParcours;
SeuilNbTermesMatrice = ProbingOuNodePresolve->SeuilNbTermesMatrice;
NombreDeContraintesAAnalyser = ProbingOuNodePresolve->NombreDeContraintesAAnalyser;
NumeroDeContrainteAAnalyser = ProbingOuNodePresolve->NumeroDeContrainteAAnalyser;
ContrainteAAnalyser = ProbingOuNodePresolve->ContrainteAAnalyser;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;   
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;
BminValide = ProbingOuNodePresolve->BminValide;
BmaxValide = ProbingOuNodePresolve->BmaxValide;
Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;

SensContrainte = Pne->SensContrainteTrav;  
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;  
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
TypeDeVariable = Pne->TypeDeVariableTrav;

/* Remarque: NombreDeContraintesAAnalyser peut varier en cours d'analyse parce qu'on peut etre amene a en ajouter.
   C'est la raison pour laquelle il faut utiliser ProbingOuNodePresolve->NombreDeContraintesAAnalyser et
	 pas seulement NombreDeContraintesAAnalyser */
	 
AnalyserListeDeContraintes:

ProbingOuNodePresolve->IndexLibreContraintesAAnalyser = 0;

for ( i = 0 ; i < ProbingOuNodePresolve->NombreDeContraintesAAnalyser ; i++ ) {
  if ( NbParcours > SeuilNbTermesMatrice ) {
		if ( Pne->AffichageDesTraces == OUI_PNE ) {
			printf("Probing stopped after checking %d elements of the matrix (max is %d)\n",NbParcours,SeuilNbTermesMatrice);
    }
		Pne->ArreterCliquesEtProbing = OUI_PNE;
	}

  Cnt = NumeroDeContrainteAAnalyser[i];
	/* On met tout de suite a non ContrainteAAnalyser */
	ContrainteAAnalyser[Cnt] = NON_PNE;
	  
	/* Que si au moins une des 2 bornes de la contrainte est connue */
	/*if ( BminValide[Cnt] != OUI_PNE && BmaxValide[Cnt] != OUI_PNE ) continue;*/
	SensCnt = SensContrainte[Cnt];
	BmnValide = BminValide[Cnt];
	BmxValide = BmaxValide[Cnt];
	Bmn = Bmin[Cnt];
	Bmx = Bmax[Cnt];
	BCnt = B[Cnt];
	/* Deja fait dans le tri des contraintes a analyser */
	/*
  if ( SensCnt == '<' ) {
	  if ( BmxValide == OUI_PNE ) {
      if ( Bmx <= BCnt + SEUIL_DADMISSIBILITE ) continue;
		}
	}
	*/
	
  ForcingContraint = PNE_DeterminerForcingConstraint( Pne, ProbingOuNodePresolve, Cnt, SensCnt, BmnValide, BmxValide, Bmn, Bmx, BCnt );						
  if ( ForcingContraint == OUI_PNE ) continue;
			
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
	NbParcours += NbTerm[Cnt];
	Nb = 0;
	Flag = 0;
  while ( il < ilMax ) {
	  Ai = A[il];
		if ( Ai == 0.0 ) goto NextIl;
		Var = Nuvar[il];		
		BrnInfConnue = BorneInfConnue[Var];
		/* Dans le cas ou la variable a ete fixee, BorneInfConnue et BorneSupConnue ont la meme valeur */
	  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
		     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) goto NextIl;				 
	  XsValide = NON_PNE;   
	  XiValide = NON_PNE;		
		Nb++;
	  Xs = ValeurDeBorneSup[Var];
	  Xi = ValeurDeBorneInf[Var];
		Xs0 = Xs;
		Xi0 = Xi;		
		
    UneVariableAEteFixee = NON_PNE;
    BorneMiseAJour = NON_PNE;
				
    if ( SensCnt == '=' ) {		
	    /* On regarde le min et le max */
	    if ( BmnValide == OUI_PNE ) {
		    BminNew = Bmn;
        if ( Ai > 0.0 ) BminNew -= Ai * Xi0; /* On avait pris le min */
        else BminNew -= Ai * Xs0; /* On avait pris le max */		
		    S = BCnt - BminNew;
		    if ( Ai > 0 ) { Xs = S / Ai; XsValide = OUI_PNE; }
	    	else { Xi = -S / fabs( Ai ); XiValide = OUI_PNE; }		
	    }
	    if ( BmxValide == OUI_PNE ) {	
        BmaxNew = Bmx;
		    if ( Ai > 0.0 ) BmaxNew -= Ai * Xs0; /* On avait pris le max */
        else BmaxNew -= Ai * Xi0; /* On avait pris le min */
        S = BCnt - BmaxNew;
		    if ( Ai > 0 ) { Xi = S / Ai; XiValide = OUI_PNE; }	
		    else { Xs = -S / fabs( Ai ); XsValide = OUI_PNE; }				
	    }	
    }
    else { /* SensContrainte est '<' */
      /* On peut calculer un majorant */			
	    if ( BmnValide == OUI_PNE ) {						
		    BminNew = Bmn;
        if ( Ai > 0.0 ) BminNew -= Ai * Xi0; /* On avait pris le min */				
        else BminNew -= Ai * Xs0; /* On avait pris le max */				
		    S = BCnt - BminNew;
		    if ( Ai > 0 ) { Xs = S / Ai; XsValide = OUI_PNE; }
		    else { Xi = -S / fabs( Ai ); XiValide = OUI_PNE; }					
	    }			 
    }
		
		/* Que si Xi ou Xs sont valides et si une des 2 bornes est plus petite ou plus grande */
		if ( XiValide != OUI_PNE && XsValide != OUI_PNE ) goto NextIl;
		if ( Xi <= Xi0 && Xs >= Xs0 ) goto NextIl;

    PNE_ModifierLaBorneDUneVariable( Pne, Var, SensCnt, XsValide, (double) Xs, XiValide, (double) Xi, &NouvelleValeur,
		                                 &BorneMiseAJour, &UneVariableAEteFixee,
		                                 &(Pne->ProbingOuNodePresolve->Faisabilite) );		
    if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;		
				
		/* Si la variable a ete fixee on met a jour la liste des contraintes a examiner au prochain coup */
		if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE  ) {		
		  Flag = 1;
      PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );			
      if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
		}
		
    NextIl:			
	  il++;
	}  

	if ( Nb == 1 && Flag == 0 ) {
    PNE_CalculXiXsContrainteAUneSeuleVariable( Pne, &Var, Cnt, Mdeb[Cnt], ilMax, A, Nuvar, BorneInfConnue, ValeurDeBorneInf, 
                                               &XiValide, &XsValide, &Xi, &Xs );
		if ( Var >= 0 ) {
      UneVariableAEteFixee = NON_PNE;
      BorneMiseAJour = NON_PNE;		
      PNE_ModifierLaBorneDUneVariable( Pne, Var, SensCnt, XsValide, (double) Xs, XiValide, (double) Xi, &NouvelleValeur,
		                                   &BorneMiseAJour, &UneVariableAEteFixee,
		                                   &(Pne->ProbingOuNodePresolve->Faisabilite) );																		 
      if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;
						
		  /* Si la variable a ete fixee on met a jour la liste des contraintes a examiner au prochain coup */
		  if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE  ) {
        PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
        if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) return;			
		  }
		}
	}
		
}
 
if ( Pne->ProbingOuNodePresolve->Faisabilite == OUI_PNE ) {
  if ( Pne->ArreterCliquesEtProbing == NON_PNE ) {
	  if ( ProbingOuNodePresolve->IndexLibreContraintesAAnalyser != 0 ) {
      ProbingOuNodePresolve->NombreDeContraintesAAnalyser = ProbingOuNodePresolve->IndexLibreContraintesAAnalyser;
			goto AnalyserListeDeContraintes;
		}
  }
}

return;

}

/*----------------------------------------------------------------------------*/


