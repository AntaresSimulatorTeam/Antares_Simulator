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

   FONCTION: On essaie de calculer des bornes sur une variable a partir
	           d'une contrainte. Utilise pour borner des variables a
						 l'aide de contraintes avec des variables dont toutes les
						 bornes sont connues sauf une.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"   

# include "prs_fonctions.h"
# include "prs_define.h" 

# include "pne_fonctions.h"  
# include "pne_define.h"

# define TRACES 0

/*----------------------------------------------------------------------------*/
/* VarTest est la variable que l'on veut essayer de borner implicitement */

void PRS_CalculeBorneSurVariableEnFonctionDeLaContrainte( PRESOLVE * Presolve, int Cnt, int VarTest,  
					                                                char * BorneInfCalculee, double * BorneInf,
																					                char * BorneSupCalculee, double * BorneSup )
{
int il; int ilMax; int Var; long double Smin; long double Smax; long double S; int * Nuvar;
double * A; int * TypeDeBornePourPresolve; long double CoeffDeVarTest;  PROBLEME_PNE * Pne;
char SminCalcule; char SmaxCalcule; int * Mdeb; int * NbTerm; double * ValeurDeXPourPresolve;
char TypeBrn; double * BorneInfPourPresolve; double * BorneSupPourPresolve; long double B;
char SensContrainte; char SeulementSminEstUtile;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
SensContrainte = Pne->SensContrainteTrav[Cnt];
B = (long double) Pne->BTrav[Cnt];

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

*BorneInfCalculee = NON_PNE;
*BorneSupCalculee = NON_PNE;
 
Smin = 0.;
Smax = 0.;
SminCalcule = OUI_PNE;
SmaxCalcule = OUI_PNE;
CoeffDeVarTest = 1.;

if ( SensContrainte == '<' ) SeulementSminEstUtile = OUI_PNE;
else SeulementSminEstUtile = NON_PNE;;

il = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
if ( SeulementSminEstUtile == OUI_PNE ) {
  while ( il < ilMax ) {
    if ( A[il] == 0.0 ) goto NextIl1;
    Var = Nuvar[il];
	  if ( Var == VarTest ) {
	    CoeffDeVarTest = (long double) A[il];
		  if ( fabs( CoeffDeVarTest ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) return;		
	    goto NextIl1;
	  }
    TypeBrn = TypeDeBornePourPresolve[Var];
	
    if ( TypeBrn == VARIABLE_NON_BORNEE ) return;
    else if ( TypeBrn == VARIABLE_FIXE ) {
      S = (long double) A[il] * (long double) ValeurDeXPourPresolve[Var];
      Smax += S;
      Smin += S;
    }
    else {
      if ( A[il] > 0. ) {			
        if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    /*SminCalcule = NON_PNE;*/
					return;
			  }
        else Smin += (long double) A[il] * (long double) BorneInfPourPresolve[Var];			
      }
      else {       			
        if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    /*SminCalcule = NON_PNE;*/
					return;
		  	}
        else Smin += (long double) A[il] * (long double) BorneSupPourPresolve[Var];
			}
    }				  
	  NextIl1:
    il++;
	}
}
else {
  while ( il < ilMax ) {
    if ( A[il] == 0.0 ) goto NextIl2;
    Var = Nuvar[il];
	  if ( Var == VarTest ) {
	    CoeffDeVarTest = (long double) A[il];
		  if ( fabs( CoeffDeVarTest ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) return;		
	    goto NextIl2;
	  }
    TypeBrn = TypeDeBornePourPresolve[Var];
	
    if ( TypeBrn == VARIABLE_NON_BORNEE ) return;
    else if ( TypeBrn == VARIABLE_FIXE ) {
      S = (long double) A[il] * (long double) ValeurDeXPourPresolve[Var];
      Smax += S;
      Smin += S;
    }
    else {
      if ( A[il] > 0. ) {
        if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    SmaxCalcule = NON_PNE;
		      if ( SminCalcule == NON_PNE ) return;				
			  }
        else Smax += (long double) A[il] * (long double) BorneSupPourPresolve[Var];
			
        if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    SminCalcule = NON_PNE;
		      if ( SmaxCalcule == NON_PNE ) return;				
			  }
        else Smin += (long double) A[il] * (long double) BorneInfPourPresolve[Var];			
      }			
      else {
        if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			    SmaxCalcule = NON_PNE;
		      if ( SminCalcule == NON_PNE ) return;
			  }
        else Smax += (long double) A[il] * (long double) BorneInfPourPresolve[Var];
			
        if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
			    SminCalcule = NON_PNE;
		      if ( SmaxCalcule == NON_PNE ) return;				
			  }
        else Smin += (long double) A[il] * (long double) BorneSupPourPresolve[Var];
			}
    }				  
	  NextIl2:
    il++;
	}
}

if ( SensContrainte == '=' ) {
  /* Dans le cas d'une contrainte d'egalite on peut calculer une borne 
     min et une borne max */ 
  if ( CoeffDeVarTest > 0.0 ) {
    if ( SminCalcule == OUI_PNE ) {
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = (double) ( ( B - Smin ) / CoeffDeVarTest );
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 			
    }
    if ( SmaxCalcule == OUI_PNE ) {
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = (double) ( ( B - Smax ) / CoeffDeVarTest );
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 					
    }
	}
	else {
    if ( SmaxCalcule == OUI_PNE ) {
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = (double) ( ( B - Smax ) / CoeffDeVarTest );
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 						
    }
    if ( SminCalcule == OUI_PNE ) {
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = (double) ( ( B - Smin ) / CoeffDeVarTest );
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 								
    }
	}
}
else {
  /* Toutes les contraintes d'inegalite ont ete transformees en <= */
  if ( CoeffDeVarTest > 0.0 ) {
    if ( SminCalcule == OUI_PNE ) {
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = (double) ( ( B - Smin ) / CoeffDeVarTest );		
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 						
    }
	}
	else {
    if ( SminCalcule == OUI_PNE ) {
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = (double) ( ( B - Smin ) / CoeffDeVarTest );				
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 								
    }
	}  
}

return;
}
