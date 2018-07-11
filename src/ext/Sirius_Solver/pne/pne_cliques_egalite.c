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

   FONCTION: On essaie de transformer les cliques en contraintes d'egalite.
	        
                 
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

void PNE_TesterFaisabiliteClique( PROBLEME_PNE *  , int , int , int * , char * , double * ,
														      double * , int * , int * , double * , int * ,
														      double * , char * , int * , int * , int * ,
                                  char * , char * , double * , double * ,
																	double * , double * , double * , double * ,
																	int * );

/*----------------------------------------------------------------------------*/

void PNE_TesterFaisabiliteClique( PROBLEME_PNE * Pne , int NombreDeContraintes,
                                  int NombreDeVariables, int * TypeDeVariable,
                                  char * BorneInfConnue, double * ValeurDeBorneInf,
														      double * ValeurDeBorneSup,
                                  int * Mdeb, int * NbTerm, double * A, int * Nuvar,
														      double * B, char * SensContrainte,
                                  int * Cdeb, int * Csui, int * NumContrainte,
                                  char * BminValide, char * BmaxValide, double * Bmin, double * Bmax,
																	double * XminSv, double * Xmin, double * XmaxSv, double * Xmax,
                                  int * Faisabilite )
{
int Cnt; int Var1; char BrnInfConnue; int Var; int il; int ilMax; char XsValide; char XiValide;
double S; double Xi; double Xs; double Ai; double BminNew; double BmaxNew; double Xs0; double Xi0;
double BCnt; char BmnValide; char BmxValide; double Bmn; double Bmx; char SensCnt;
int ic; double NouvelleValeur; char BorneMiseAJour; char UneVariableAEteFixee; int Nb;

/* Contraintes a une seule variable: on essaie de les fixer ou d'ameliorer la borne de
   la variable sans la distinction du type entier ou continu */

*Faisabilite = OUI_PNE; 
for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {

continue;

  Nb = 0;
  Var1 = -1;
	S = 0.0;
	Ai = 1;
  il = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
	  if ( A[il] == 0.0 ) goto NextElement;
    Var = Nuvar[il];		
    BrnInfConnue = BorneInfConnue[Var];
    if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	       BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) {
	    S += A[il] * ValeurDeBorneInf[Var];
			goto NextElement;
		}
  	Nb++;
		if ( Nb > 1 ) break;
		Var1 = Var;
		Ai = A[il];
	  NextElement:
	  il++;
  }		
  if ( Nb != 1 ) continue;  
	Var = Var1;
  XsValide = NON_PNE; 
  XiValide = NON_PNE;
	Xi = ValeurDeBorneInf[Var];
	Xs = ValeurDeBorneSup[Var];
  S = B[Cnt] - S;
  if ( SensContrainte[Cnt] == '=' ) {
    XiValide = OUI_PNE;
    XsValide = OUI_PNE;
    Xi = S / Ai;
	  Xs = Xi;
  }
  else {
	  if ( Ai > 0 ) { Xs = S / Ai; XsValide = OUI_PNE; }
	  else          { Xi = -S / fabs( Ai ); XiValide = OUI_PNE; }									 
  }
  UneVariableAEteFixee = NON_PNE;
  BorneMiseAJour = NON_PNE;
  PNE_ModifierLaBorneDUneVariable( Pne, Var, SensContrainte[Cnt], XsValide, Xs, XiValide, Xi, &NouvelleValeur,
		                               &BorneMiseAJour, &UneVariableAEteFixee, Faisabilite );															 
	if ( *Faisabilite == NON_PNE ) {
	  return;
	}
	
	/* Si la variable a ete fixee ou une borne mise a jour on modifie les bornes des contraintes */
	if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE  ) {
    PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, XminSv,
	                                   XmaxSv, Xmin, Xmax, Var, NouvelleValeur, Faisabilite );																		 
  	if ( *Faisabilite == NON_PNE ) {
		  return;
		}
	}
}

/* Apres avoir balaye les contraintes a une seule variable, on examine les variables
   colonne par colonne et on essaie de les fixer */	 
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeVariable[Var] != ENTIER ) continue;
	BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;	 					
  ic = Cdeb[Var];
  while ( ic >= 0 ) {
    Ai = A[ic];
    if ( Ai == 0.0 ) goto NextIc;		
    Cnt = NumContrainte[ic];
	  SensCnt = SensContrainte[Cnt];
	  BmnValide = BminValide[Cnt];
	  BmxValide = BmaxValide[Cnt];
	  Bmn = Bmin[Cnt];
	  Bmx = Bmax[Cnt];
	  BCnt = B[Cnt];
    if ( SensCnt == '<' ) {
	    if ( BmxValide == OUI_PNE ) {
        if ( Bmx <= BCnt + SEUIL_DADMISSIBILITE ) goto NextIc;
		  }
	  }		
		
		XsValide = NON_PNE;
		XiValide = NON_PNE;
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
        else BminNew -= Ai * Xs0; /* On avait pris le mas */		
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
		if ( XiValide != OUI_PNE && XsValide != OUI_PNE ) goto NextIc;
		if ( Xi <= Xi0 && Xs >= Xs0 ) goto NextIc;				
    PNE_ModifierLaBorneDUneVariable( Pne, Var, SensCnt, XsValide, Xs, XiValide, Xi, &NouvelleValeur,
		                                 &BorneMiseAJour, &UneVariableAEteFixee, Faisabilite );
    if ( *Faisabilite == NON_PNE ) {
		  return;
		}
		goto NextIc;
		   
	  /* Si la variable a ete fixee ou une borne mise a jour on modifie les bornes des contraintes */																	 
	  if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE  ) {		  
	    *Faisabilite = OUI_PNE;
      PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, XminSv,
	                                     XmaxSv, Xmin, Xmax, Var, NouvelleValeur, Faisabilite );			
  	  if ( *Faisabilite == NON_PNE ) {			
			  return;
			}
	  }	 
	  NextIc:
    ic = Csui[ic];
  }
}

return;
}

/*----------------------------------------------------------------------------*/

void PNE_TransformerCliquesEnEgalites( PROBLEME_PNE * Pne )
{
int NombreDeVariables; double * X; double * Xmin; double * Xmax; char BorneMiseAJour;
CLIQUES * Cliques; int c; int NombreDeContraintes; double ValeurDeVar; int ilDeb;
int * First; int * NbElements; int il; int ilMax; int Faisabilite; int Pivot;  
int Var; int * NoeudDeClique; int * TypeDeBorne; char Ok; double * Bmin; double * Bmax;
double * BminSv; double * BmaxSv; double * XminSv; double * XmaxSv; int * Cdeb;
int * Csui; int * NumContrainte; double * A; int * TypeDeVariable;
char * BorneInfConnue; double * ValeurDeBorneInf; double * ValeurDeBorneSup;
char * BminValide; char * BmaxValide; int * Mdeb; int * NbTerm; int * Nuvar;
double * B; char * SensContrainte;

if ( Pne->ConflictGraph == NULL ) return;
if ( Pne->Cliques == NULL ) return;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
X = Pne->UTrav;
Xmin = Pne->UminTrav;
Xmax = Pne->UmaxTrav;
XminSv = Pne->UminTravSv;
XmaxSv = Pne->UmaxTravSv;

Cdeb = Pne->CdebTrav;   
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;

Cliques = Pne->Cliques;
First = Cliques->First;  
NbElements = Cliques->NbElements;
NoeudDeClique = Cliques->Noeud;
Pivot = Pne->ConflictGraph->Pivot;

BminValide = Pne->ProbingOuNodePresolve->BminValide;
BmaxValide = Pne->ProbingOuNodePresolve->BmaxValide;
Bmin = Pne->ProbingOuNodePresolve->Bmin;
Bmax = Pne->ProbingOuNodePresolve->Bmax;
BminSv = Pne->ProbingOuNodePresolve->BminSv;
BmaxSv = Pne->ProbingOuNodePresolve->BmaxSv;

BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
ValeurDeBorneSup = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;

/* Calcul des bornes sur les contraintes */

memcpy( (char * ) XminSv, (char * ) Xmin, NombreDeVariables * sizeof( double ) );  
memcpy( (char * ) XmaxSv, (char * ) Xmax, NombreDeVariables * sizeof( double ) );  

PNE_InitBorneInfBorneSupDesVariables( Pne );
PNE_CalculMinEtMaxDesContraintes( Pne, &Faisabilite );
if ( Faisabilite == NON_PNE ) return;

/* Et on sauvegarde le resultat comme point de depart pour les noeuds suibvants */
memcpy( (char *) BminSv, (char *) Bmin, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) BmaxSv, (char *) Bmax, NombreDeContraintes * sizeof( double ) );

/* On initialise a 0 toutes les variables de la clique inferieures a Pivot
   et a 1 toutes les variables de la clique superieures ou egal a pivot.
	 S'il n'y a pas de solution admissibles alors la clique peut etre transformee
	 en contraintes d'egalite */
	 	 
for ( c = 0 ; c < Cliques->NombreDeCliques ; c++ ) {
	
  ilDeb = First[c];
	if ( ilDeb < 0 ) continue; /* On ne sait jamais ... */

  Pne->ProbingOuNodePresolve->Faisabilite = OUI_PNE;		
  Pne->ProbingOuNodePresolve->NombreDeVariablesFixees = 0;

  /* On remet des bornes inf et sup des variables */
  memcpy( (char *) Xmin, (char *) XminSv, NombreDeVariables * sizeof( double ) );
  memcpy( (char *) Xmax, (char *) XmaxSv, NombreDeVariables * sizeof( double ) );
	
	il = ilDeb;
  ilMax = il + NbElements[c];
	Ok = OUI_PNE;
  while ( il < ilMax ) {
	  if ( NoeudDeClique[il] < Pivot ) {
	    if ( TypeDeBorne[NoeudDeClique[il]] == VARIABLE_FIXE ) {				
				Ok = NON_PNE;
			  break;
			}			
			/* On initialise a 0 */
			Var = NoeudDeClique[il];
      Xmin[Var] = 0.0;
      Xmax[Var] = 0.0;			
		}
		else {
	    if ( TypeDeBorne[NoeudDeClique[il]-Pivot] == VARIABLE_FIXE ) {				
				Ok = NON_PNE;
			  break;   
			}			
			/* On initialise a 1 */
			Var = NoeudDeClique[il]-Pivot;
      Xmin[Var] = 1.0;
      Xmax[Var] = 1.0;
		}		
	  il++;
	}
	
	if ( Ok == NON_PNE ) continue;	
	
  PNE_InitBorneInfBorneSupDesVariables( Pne );
	
  /* On recupere les bornes des contraintes au noeud racine */
  memcpy( (char *) Bmin, (char *) BminSv, NombreDeContraintes * sizeof( double ) );
  memcpy( (char *) Bmax, (char *) BmaxSv, NombreDeContraintes * sizeof( double ) );
	
	il = ilDeb;
  while ( il < ilMax ) {
	  if ( NoeudDeClique[il] < Pivot ) {
			/* On initialise a 0 */
			Var = NoeudDeClique[il];
		  ValeurDeVar = 0;
		  BorneMiseAJour = MODIF_BORNE_SUP;				
		}
		else {
			/* On initialise a 1 */
			Var = NoeudDeClique[il]-Pivot;
		  ValeurDeVar = 1;
		  BorneMiseAJour = MODIF_BORNE_INF;			
		}

    /* On met a jour les contraintes dans lesquelles la variable intervient */		
    PNE_NodePresolveMajBminBmax( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, XminSv, XmaxSv,
                                 Var, ValeurDeVar, BorneMiseAJour );
																 		
	  il++;
	}
	
	/* On applique le graphe de conflits */
	il = ilDeb;
  while ( il < ilMax ) {
	  if ( NoeudDeClique[il] < Pivot ) {
		  /* On initialise a 0 */
			Var = NoeudDeClique[il];
		  ValeurDeVar = 0;			
		}
		else {		
			/* On initialise a 1 */
			Var = NoeudDeClique[il]-Pivot;
		  ValeurDeVar = 1;			     		
		}
    Faisabilite = OUI_PNE;
    PNE_NodePresolveGrapheDeConflit( Pne, Cdeb, Csui, NumContrainte, A, Bmin, Bmax, XminSv,
	                                   XmaxSv, Xmin, Xmax, Var, ValeurDeVar, &Faisabilite );
    if ( Faisabilite == NON_PNE ) {
		  printf("Pas de solution apres NodePresolveGrapheDeConflit \n");
      Cliques->CliqueDeTypeEgalite[c] = OUI_PNE;
 			Ok = NON_PNE;
			break;
		}	
	  il++;			   
  }
	/* On verifie la faisabilite */
	if ( Ok == OUI_PNE ) {
    Faisabilite = OUI_PNE;
    PNE_TesterFaisabiliteClique(  Pne , NombreDeContraintes, NombreDeVariables, TypeDeVariable,
                                  BorneInfConnue, ValeurDeBorneInf, ValeurDeBorneSup,
                                  Mdeb, NbTerm, A, Nuvar, B, SensContrainte,
                                  Cdeb, Csui, NumContrainte, BminValide, BmaxValide, Bmin, Bmax,
																 	XminSv, Xmin, XmaxSv, Xmax,
                                  &Faisabilite );
		
    if ( Faisabilite == NON_PNE ) {
		  printf("Pas de solution apres TesterFaisabiliteClique \n");
      Cliques->CliqueDeTypeEgalite[c] = OUI_PNE;		
	  }
		else printf("solution\n");
		
	}
	
}

Pne->ProbingOuNodePresolve->Faisabilite = OUI_PNE;		

memcpy( (char *) Xmin, (char *) XminSv, NombreDeVariables * sizeof( double ) );
memcpy( (char *) Xmax, (char *) XmaxSv, NombreDeVariables * sizeof( double ) );

memcpy( (char *) Bmin, (char *) BminSv, NombreDeContraintes * sizeof( double ) );
memcpy( (char *) Bmax, (char *) BmaxSv, NombreDeContraintes * sizeof( double ) );
	
return;
}
