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

   FONCTION: On essaie de calculer des bornes sur une variable duale a 
	           partir d'une variable. Utilise pour borner des variables
						 duale a l'aide de variables dont les contraintes qui entrent
						 dans le calcul du cout reduit on des variables duales
						 connues ou bornees sauf une.
                
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

void PRS_CalculeBorneSurVariableDualeEnFonctionDeLaVariable( PRESOLVE * Presolve, int CntTest, int Var,  
					                                                   char TypeBrn,
																														 char * BorneInfCalculee, double * BorneInf,
																					                   char * BorneSupCalculee, double * BorneSup )
{
int ic; double C; double CoeffDeCntTest; double X; int Cnt; double Smin; double Smax;
int * Cdeb; int * Csui; int * NumContrainte; double * A; double * LambdaMin;
double * LambdaMax; double * Lambda; char * ConnaissanceDeLambda; char * ContrainteInactive;
char SminCalcule; char SmaxCalcule; char SensCoutReduit; PROBLEME_PNE * Pne;
 
Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*BorneInfCalculee = NON_PNE;
*BorneSupCalculee = NON_PNE;

if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) SensCoutReduit = '>';
else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) SensCoutReduit = '<';
else if ( TypeBrn == VARIABLE_NON_BORNEE ) SensCoutReduit = '=';
else return;

C = Pne->LTrav[Var];
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;

LambdaMin = Presolve->LambdaMin;
LambdaMax = Presolve->LambdaMax;
Lambda = Presolve->Lambda;
ConnaissanceDeLambda = Presolve->ConnaissanceDeLambda;
ContrainteInactive = Presolve->ContrainteInactive;
 
Smin = 0.;
Smax = 0.;
SminCalcule = OUI_PNE;
SmaxCalcule = OUI_PNE;
CoeffDeCntTest = 1.;

ic = Cdeb[Var];
while ( ic >= 0 ) {

  if ( A[ic] == 0.0 ) goto NextIc;
  Cnt = NumContrainte[ic];
	
  if ( ContrainteInactive[Cnt] == OUI_PNE ) goto NextIc;

	if ( Cnt == CntTest ) {
    CoeffDeCntTest = A[ic];		
		if ( fabs( CoeffDeCntTest ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) return;		
    goto NextIc;
	}

	if ( ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU ) {
    X = A[ic] * Lambda[Cnt];
    Smin += X;
    Smax += X;
	}
	else {
  	if ( A[ic] > 0.0 ) {
			/* MaxSommeDesAiFoisLambda */
		  if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
				Smax += A[ic] * LambdaMax[Cnt];
			}
			else SmaxCalcule = NON_PNE;
			/* MinSommeDesAiFoisLambda */				
		  if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
				Smin += A[ic] * LambdaMin[Cnt];
      }
      else SminCalcule = NON_PNE;
  			
		}	
		else {
      /* A[ic] < 0 */
			/* MaxSommeDesAiFoisLambda */			
		  if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
				Smax += A[ic] * LambdaMin[Cnt];
			}
			else SmaxCalcule = NON_PNE;      
			/* MinSommeDesAiFoisLambda */								
		  if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
				Smin += A[ic] * LambdaMax[Cnt];
			}
      else SminCalcule = NON_PNE;        				
	  }
	}			
  NextIc:
	if ( SminCalcule == NON_PNE && SmaxCalcule == NON_PNE ) break;
  ic = Csui[ic];
}

if ( SensCoutReduit == '=' ) {
  /* Dans le cas d'une contrainte d'egalite on peut calculer une borne min et une borne max */
	/* On a uA = c */
  if ( CoeffDeCntTest > 0.0 ) {	
    if ( SminCalcule == OUI_PNE ) { 		
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = ( C - Smin ) / CoeffDeCntTest ;
			
      # if TRACES == 1
		 	  printf("Calcul d'une BorneSup sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneSup);
      # endif
			
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 			
    }
    if ( SmaxCalcule == OUI_PNE ) {		
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = ( C - Smax ) / CoeffDeCntTest;

      # if TRACES == 1
		 	  printf("Calcul d'une BorneInf sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneInf);
      # endif
						
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 					
    }
	}
	else {
    if ( SmaxCalcule == OUI_PNE ) {		
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = ( C - Smax ) / CoeffDeCntTest;
			
      # if TRACES == 1
		 	  printf("Calcul d'une BorneSup sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneSup);
      # endif
			
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 						
    }
    if ( SminCalcule == OUI_PNE ) {					
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = ( C - Smin ) / CoeffDeCntTest;

      # if TRACES == 1
		 	  printf("Calcul d'une BorneInf sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneInf);
      # endif
								
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 								
    }
	}
}
else if ( SensCoutReduit == '>' ) {
  /* On doit avoir c-uA >= 0 <=> uA <= c */
  if ( CoeffDeCntTest > 0.0 ) {
    if ( SminCalcule == OUI_PNE ) {		
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = ( C - Smin ) / CoeffDeCntTest;
			
      # if TRACES == 1
		 	  printf("Calcul d'une BorneSup sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneSup);
      # endif
						
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 						
    }
	}
	else {
    if ( SminCalcule == OUI_PNE ) {		
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = ( C - Smin ) / CoeffDeCntTest;

      # if TRACES == 1
		 	  printf("Calcul d'une BorneInf sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneInf);
      # endif
			
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 								
    }
	}  
}
else if ( SensCoutReduit == '<' ) {
  /* On doit avoir c-uA <= 0 <=> uA >= c */
  if ( CoeffDeCntTest > 0.0 ) {
    if ( SmaxCalcule == OUI_PNE ) {		
	    *BorneInfCalculee = OUI_PNE;
		  *BorneInf = ( C - Smax ) / CoeffDeCntTest;
			
      # if TRACES == 1
		 	  printf("Calcul d'une BorneInf sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneInf);
      # endif
			
      if ( fabs( *BorneInf ) > VALEUR_DE_BORNE_DEBILE ) *BorneInfCalculee = NON_PNE; 						
    }
	}
	else {
    if ( SmaxCalcule == OUI_PNE ) {		
	    *BorneSupCalculee = OUI_PNE;
		  *BorneSup = ( C - Smax ) / CoeffDeCntTest;
			
      # if TRACES == 1
		 	  printf("Calcul d'une BorneSup sur la variable duale de la contrainte %d: %e\n",CntTest,*BorneSup);
      # endif
									
      if ( fabs( *BorneSup ) > VALEUR_DE_BORNE_DEBILE ) *BorneSupCalculee = NON_PNE; 						
    }
	}
}

return;
}
