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

   FONCTION: Presolve simplifie. Appele par le reduced cost fixing au noeud
	           racine et par le noede presolve.
                
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

# define MX_CYCLES_REDUCED_COST_FIXING_AU_NOEUD_RACINE     5
# define MX_CYCLES_REDUCED_COST_FIXING_POUR_NODE_PRESOLVE  5

# define TRACES 0

# define PRISE_EN_COMPTE_DES_CONTRAINTES_DE_BORNES_VARIABLES NON_PNE

# define POURCENT_LIMITE 0.01
# define SEUIL_LIMITE    10

/*----------------------------------------------------------------------------*/
/* Appele a chaque fois qu'on trouve une solution entiere */
void PNE_PresolveSimplifie( PROBLEME_PNE * Pne, char * ContrainteActivable, char Mode, int * Faisabilite )														
{
int Var; int * TypeDeBorne; int * TypeDeVariable; double S; double * Bmin; double * Bmax;
char * BminValide; char * BmaxValide; char BmnValide; char BmxValide; char SensCnt;
double Bmn; double Bmx; double BCnt; double Ai; double * A; double Xs; double Xi;
double Xs0; double Xi0; int NombreDeVariables; double BminNew; double BmaxNew; int i;
double NouvelleValeur; char * SensContrainte; double * B; int * Cdeb; int * Csui; int ic;
int * NumContrainte; int Cnt; int NombreDeContraintes;  char ForcingConstraint;
char * BorneSupConnue; double * ValeurDeBorneSup; char * BorneInfConnue; double * ValeurDeBorneInf;
char BrnInfConnue; char BorneMiseAJour; char UneVariableAEteFixee; char RefaireUnCycle;
char XsValide; char XiValide; int NbCycles; PROBING_OU_NODE_PRESOLVE * ProbingOuNodePresolve;
char SigneCoeff; double * L; int NombreDeVariablesNonFixes; int * NumeroDesVariablesNonFixes;
double CoutVar; int MxCycles; int * CNbTerm; int Limite;

/*printf("PresolveSimplifie Mode = %d\n",Mode);*/

ProbingOuNodePresolve = Pne->ProbingOuNodePresolve;
if ( ProbingOuNodePresolve == NULL ) return;

NombreDeVariables = Pne->NombreDeVariablesTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
NombreDeVariablesNonFixes = Pne->NombreDeVariablesNonFixes;
NumeroDesVariablesNonFixes = Pne->NumeroDesVariablesNonFixes;

L = Pne->LTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
TypeDeBorne = Pne->TypeDeBorneTrav;

BorneSupConnue = ProbingOuNodePresolve->BorneSupConnue;
ValeurDeBorneSup = ProbingOuNodePresolve->ValeurDeBorneSup;
BorneInfConnue = ProbingOuNodePresolve->BorneInfConnue;
ValeurDeBorneInf = ProbingOuNodePresolve->ValeurDeBorneInf;

Bmin = ProbingOuNodePresolve->Bmin;
Bmax = ProbingOuNodePresolve->Bmax;
BminValide = ProbingOuNodePresolve->BminValide;
BmaxValide = ProbingOuNodePresolve->BmaxValide;

SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;

Cdeb = Pne->CdebTrav;
CNbTerm = Pne->CNbTermTrav;
Csui = Pne->CsuiTrav;
A = Pne->ATrav;
NumContrainte = Pne->NumContrainteTrav;

if ( Mode == PRESOLVE_SIMPLIFIE_POUR_NODE_PRESOLVE ) MxCycles = MX_CYCLES_REDUCED_COST_FIXING_POUR_NODE_PRESOLVE;
else MxCycles = MX_CYCLES_REDUCED_COST_FIXING_AU_NOEUD_RACINE;

Pne->ProbingOuNodePresolve->Faisabilite = OUI_PNE;		
NbCycles = 0;
DebutDeCycle:
	
RefaireUnCycle = NON_PNE;

/* Forcing constraints et contraintes inactives */

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {  
 	if ( ContrainteActivable[Cnt] == NON_PNE ) continue;			
  ForcingConstraint = PNE_DeterminerForcingConstraint( Pne, ProbingOuNodePresolve, Cnt, SensContrainte[Cnt],
	                                                     BminValide[Cnt], BmaxValide[Cnt], Bmin[Cnt], Bmax[Cnt], B[Cnt] );						
  if ( ForcingConstraint == OUI_PNE ) {
	  ContrainteActivable[Cnt] = NON_PNE;
 		continue;
 	}
 	if ( SensContrainte[Cnt] == '<' ) {
 		if ( BmaxValide[Cnt] == OUI_PNE ) {
      if ( Bmax[Cnt] < B[Cnt] - 1.e-6 ) {
	 		  ContrainteActivable[Cnt] = NON_PNE;
  			continue;
		 	}
		}
	}	
}

/* Essayer aussi les contraintes a une seule variable pour fixer des variables avec Bmin Bmax non calculables cf le Node Presolve REF */

Limite = (int) ( POURCENT_LIMITE * NombreDeContraintes );
if ( Limite < SEUIL_LIMITE ) Limite = SEUIL_LIMITE;

for ( i = 0 ; i < NombreDeVariablesNonFixes ; i++ ) {

  Var = NumeroDesVariablesNonFixes[i];

	/*printf(" attention mise en commentaire de l'instruction qui suit dans PNE_PresolveSimplifie \n");*/
	/*if ( Mode == PRESOLVE_SIMPLIFIE_POUR_NODE_PRESOLVE && TypeDeVariable[Var] != ENTIER ) continue;*/	
	if ( Mode == PRESOLVE_SIMPLIFIE_POUR_NODE_PRESOLVE && TypeDeVariable[Var] != ENTIER ) {
    if ( CNbTerm[Var] > Limite ) continue;		
	}	
	
  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;

	CoutVar = L[Var];
	SigneCoeff = '?';

  ic = Cdeb[Var];
  while ( ic >= 0 ) {
    Ai = A[ic];
    if ( Ai == 0.0 ) goto NextIc;		
    Cnt = NumContrainte[ic];
    if ( ContrainteActivable[Cnt] == NON_PNE ) goto NextIc;
		
	  SensCnt = SensContrainte[Cnt];

		/* Pour la fixation sur critere */
	  /* On ne fait la fixation sur critere que si le cout de la variable est nul */
    if ( CoutVar == 0.0 && SigneCoeff != 'X' ) {				 
      if ( SensCnt == '=' ) SigneCoeff = 'X';
			else {
		    if ( Ai > 0.0 ) {
          if ( SigneCoeff == '?' ) SigneCoeff = '+';
			    else if ( SigneCoeff == '-' ) SigneCoeff = 'X';				
			  }		  
		    else {
          if ( SigneCoeff == '?' ) SigneCoeff = '-';
			    else if ( SigneCoeff == '+' ) SigneCoeff = 'X';
			  }
		  }		
    }
		
	  BmnValide = BminValide[Cnt];
	  BmxValide = BmaxValide[Cnt];
		if ( BmnValide == NON_PNE && BmnValide == NON_PNE ) goto NextIc;

	  Bmn = Bmin[Cnt];
	  Bmx = Bmax[Cnt];
	  BCnt = B[Cnt];

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
		  # if TRACES == 1
			  printf("Pas de solution dans le presolve simplifie\n");
			# endif			
		  return;
		}		
	  /* Si la variable a ete fixee ou une borne mise a jour on modifie les bornes des contraintes */																	 
	  if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE  ) {		  
		  # if TRACES == 1
			  printf("Variable %d  NouvelleValeur de borne %e\n",Var,NouvelleValeur);
			# endif			
      PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
      if ( Pne->ProbingOuNodePresolve->Faisabilite == NON_PNE ) *Faisabilite = NON_PNE;
  	  if ( *Faisabilite == NON_PNE ) {
		    # if TRACES == 1
			    printf("Pas de solution dans le presolve simplifie\n");
			  # endif				
		    return;
			}			
		  RefaireUnCycle = OUI_PNE;
			if ( UneVariableAEteFixee != NON_PNE ) {
			  break;
			}
	  }											 		
	  NextIc:
    ic = Csui[ic];
  }

	if ( UneVariableAEteFixee != NON_PNE ) continue;

	/* Fixation sur critere */
	if ( CoutVar != 0.0 ) continue;
	
	if ( SigneCoeff == 'X' ) continue;	

  BrnInfConnue = BorneInfConnue[Var];
  if ( BrnInfConnue == FIXE_AU_DEPART || BrnInfConnue == FIXATION_SUR_BORNE_INF ||
	     BrnInfConnue == FIXATION_SUR_BORNE_SUP || BrnInfConnue == FIXATION_A_UNE_VALEUR ) continue;
			 
	NouvelleValeur = -1;
	BorneMiseAJour = NON_PNE;
  UneVariableAEteFixee = NON_PNE;				 

	if ( SigneCoeff == '+' ) {
	  if ( CoutVar >= 0.0 ) {
		  /* On fixe a Umin */
			if ( BorneInfConnue[Var] == OUI_PNE ) {				
		    # if TRACES == 1
			    printf("Fixation de la variable %d a %e\n",Var,ValeurDeBorneInf[Var]);
	      # endif								
        NouvelleValeur = ValeurDeBorneInf[Var];
				UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;				
			}
		}  
	}	
	else if ( SigneCoeff == '-' ) {
	  if ( CoutVar <= 0.0 ) {
			/* On fixe a Umax */
			if ( BorneSupConnue[Var] == OUI_PNE ) {				
		    # if TRACES == 1
			    printf("Fixation de la variable %d a %e\n",Var,ValeurDeBorneSup[Var]);
	      # endif				
        NouvelleValeur = ValeurDeBorneSup[Var];
				UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;				      									
			}
		}
	}
	else if ( SigneCoeff == '?' ) {
    /* La variable n'apparait pas dans les contraintes */
	  if ( CoutVar >= 0.0 ) {
		  /* On fixe a Umin */
			if ( BorneInfConnue[Var] == OUI_PNE ) {
		    # if TRACES == 1
			    printf("Fixation de la variable %d a %e\n",Var,ValeurDeBorneInf[Var]);
	      # endif
        NouvelleValeur = ValeurDeBorneInf[Var];
				UneVariableAEteFixee = FIXATION_SUR_BORNE_INF;				       
			}
		}
		else {
			if ( BorneSupConnue[Var] == OUI_PNE ) {
		    # if TRACES == 1
			    printf("Fixation de la variable %d a %e\n",Var,ValeurDeBorneSup[Var]);
	      # endif
        NouvelleValeur = ValeurDeBorneSup[Var];
			  UneVariableAEteFixee = FIXATION_SUR_BORNE_SUP;				  
			}
		}
  }
	if ( UneVariableAEteFixee != NON_PNE || BorneMiseAJour != NON_PNE ) {
		# if TRACES == 1
			printf("Variable %d  NouvelleValeur de borne %e\n",Var,NouvelleValeur);
		# endif			
    PNE_VariableProbingAppliquerLeConflictGraph( Pne, Var, NouvelleValeur, BorneMiseAJour, UneVariableAEteFixee );
    if ( ProbingOuNodePresolve->Faisabilite == NON_PNE ) *Faisabilite = NON_PNE;
  	if ( *Faisabilite == NON_PNE ) {
		  # if TRACES == 1
			  printf("Pas de solution dans le presolve simplifie\n");
			# endif			
		  return;
		} 		
	  RefaireUnCycle = OUI_PNE;		
  }			
	
}

if ( Mode == PRESOLVE_SIMPLIFIE_POUR_NODE_PRESOLVE ) goto FIN_FORCING;

/* Forcing constraints et contraintes inactives */

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {  
 	if ( ContrainteActivable[Cnt] == NON_PNE ) continue;
  ForcingConstraint = PNE_DeterminerForcingConstraint( Pne, ProbingOuNodePresolve, Cnt, SensContrainte[Cnt],
	                                                     BminValide[Cnt], BmaxValide[Cnt], Bmin[Cnt], Bmax[Cnt], B[Cnt] );						
  if ( ForcingConstraint == OUI_PNE ) {
	  ContrainteActivable[Cnt] = NON_PNE;
 	  RefaireUnCycle = OUI_PNE;
 		continue;
 	}
 	if ( SensContrainte[Cnt] == '<' ) {
 		if ( BmaxValide[Cnt] == OUI_PNE ) {
      if ( Bmax[Cnt] < B[Cnt] - 1.e-6 ) {
	 		  ContrainteActivable[Cnt] = NON_PNE;
 		    RefaireUnCycle = OUI_PNE;
  			continue;
		 	}
		}
	}	
}

FIN_FORCING:

   
/* Test */
//if ( Mode == PRESOLVE_SIMPLIFIE_POUR_REDUCED_COST_FIXING_AU_NOEUD_RACINE && 0 ) { 
  /*printf("Test PNE_PresolveSimplifieVariableProbin  en cours de mise au point il suffit de ne pas l'appeler si pas bon\n");*/
  /*PNE_PresolveSimplifieVariableProbing( Pne, Faisabilite, &RefaireUnCycle );*/
//}
/* Fin test */

/* Test */
# if PRISE_EN_COMPTE_DES_CONTRAINTES_DE_BORNES_VARIABLES == OUI_PNE
  # if CONSTRUIRE_BORNES_VARIABLES == OUI_PNE
    if ( Mode == PRESOLVE_SIMPLIFIE_POUR_REDUCED_COST_FIXING_AU_NOEUD_RACINE || 1 ) { 
      PNE_PresolveSimplifieContraintesDeBornesVariables( Pne, Faisabilite, &RefaireUnCycle );
  	  if ( *Faisabilite == NON_PNE ) {
		    return;
      }		
	  }
  # endif
# endif
/* Fin test */


if ( RefaireUnCycle == OUI_PNE ) {
	NbCycles++;
  if ( NbCycles < MxCycles ) {
    # if TRACES == 1
	    printf("-> Rebouclage dans de reduced cost fixing au noeud racine\n");
	  # endif
	  goto DebutDeCycle;
	}
}

return;
}  

/*-------------------------------------------------------------------------------*/
