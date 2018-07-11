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

   FONCTION: Amelioration des bornes sur les variables duales.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"

# include "pne_define.h"

# include "prs_define.h"

# define VALEUR_NULLE 1.e-8
# define TRACES 0
																	   					
/*----------------------------------------------------------------------------*/

void PRS_AmeliorerBornesDesVariablesDuales( PRESOLVE * Presolve,
                                            int Var,
																						char TypeBrn,
																						double MinSommeDesAiFoisLambda,
																						char MinSommeDesAiFoisLambdaValide,
																						double MaxSommeDesAiFoisLambda,
																						char MaxSommeDesAiFoisLambdaValide,
																						int * NbModifications )
{
int il; int Cnt; double CoeffDeVar; PROBLEME_PNE * Pne; double * LambdaMin;
double * LambdaMax; double * Lambda; int * Cdeb; int * Csui; int * NumContrainte;
char * ContrainteInactive; char * ConnaissanceDeLambda; double * A; double X;
char SensCoutReduit; double CBarreMin; double CBarreMax;

if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) SensCoutReduit = '>';
else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) SensCoutReduit = '<';
else if ( TypeBrn == VARIABLE_NON_BORNEE ) SensCoutReduit = '=';
else return;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;    

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;

CBarreMin = Pne->LTrav[Var] - MaxSommeDesAiFoisLambda;
CBarreMax = Pne->LTrav[Var] - MinSommeDesAiFoisLambda;

LambdaMin = Presolve->LambdaMin;
LambdaMax = Presolve->LambdaMax;
Lambda = Presolve->Lambda;
ConnaissanceDeLambda = Presolve->ConnaissanceDeLambda;
ContrainteInactive = Presolve->ContrainteInactive;

if ( MinSommeDesAiFoisLambdaValide == OUI_PNE ) {
  if ( SensCoutReduit == '>' || SensCoutReduit == '=' ) {
	  /* Le cout reduit est soit positif ou nul, soit nul */
    il = Cdeb[Var];
    while ( il >= 0 ) {
      Cnt = NumContrainte[il];
      if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante_1;
	    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU ) goto ContrainteSuivante_1;
      CoeffDeVar = A[il];
	    if ( fabs( CoeffDeVar ) < VALEUR_NULLE ) goto ContrainteSuivante_1;			
	    if ( CoeffDeVar > 0.0 ) {
		    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {					 
		      X = ( CBarreMax / CoeffDeVar ) + LambdaMin[Cnt];									
			    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
			      if ( X < LambdaMax[Cnt] ) {
              LambdaMax[Cnt] = X;
							*NbModifications = *NbModifications + 1;
						  # if TRACES == 1
                printf("LambdaMax ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
						  # endif
			      }
				  }
				  else {
				    /* Borne sur LambdaMax */
            LambdaMax[Cnt] = X;
						*NbModifications = *NbModifications + 1;
					  # if TRACES == 1
              printf("LambdaMax ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
					  # endif
            if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU ) ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_ET_MAX_CONNU;
			      else ConnaissanceDeLambda[Cnt] = LAMBDA_MAX_CONNU;					
				  }
			  }						
		  }
		  else {
		    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {					 		
		      X = ( CBarreMax / CoeffDeVar ) + LambdaMax[Cnt];							
			    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {				
			      if ( X > LambdaMin[Cnt] ) {
              LambdaMin[Cnt] = X;
						  *NbModifications = *NbModifications + 1;
						  # if TRACES == 1
                printf("LambdaMin ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
					    # endif
			      }
				  }
				  else {
				    /* Borne sur LambdaMin */
            LambdaMin[Cnt] = X;
						*NbModifications = *NbModifications + 1;
					  # if TRACES == 1
              printf("LambdaMin ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
					  # endif
            if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU ) ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_ET_MAX_CONNU;
			      else ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_CONNU;
				  }
			  }
		  }
      if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
        if ( fabs( LambdaMin[Cnt] - LambdaMax[Cnt] ) < VALEUR_NULLE ) {	
	        Lambda[Cnt] = 0.5 * ( LambdaMin[Cnt] + LambdaMax[Cnt] );
		      LambdaMin[Cnt] = Lambda[Cnt];
		      LambdaMax[Cnt] = Lambda[Cnt];
          ConnaissanceDeLambda[Cnt] = LAMBDA_CONNU;
					*NbModifications = *NbModifications + 1;
	      }
      }		
      ContrainteSuivante_1:	
      il = Csui[il];
	  }
  }
}

if ( MaxSommeDesAiFoisLambdaValide == OUI_PNE ) { 
  if ( SensCoutReduit == '<' || SensCoutReduit == '=' ) {
	  /* Le cout reduit est soit negatif ou nul, soit nul */
    il = Cdeb[Var];
    while ( il >= 0 ) {
      Cnt = NumContrainte[il];
      if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante_2;
	    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_CONNU ) goto ContrainteSuivante_2;
      CoeffDeVar = A[il];
	    if ( fabs( CoeffDeVar ) < VALEUR_NULLE ) goto ContrainteSuivante_2;			
	    if ( CoeffDeVar > 0.0 ) {
		    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {					 		
			    X = ( CBarreMin / CoeffDeVar ) + LambdaMax[Cnt];						
			    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {							
			      if ( X > LambdaMin[Cnt] ) {
              LambdaMin[Cnt] = X;
				     	*NbModifications = *NbModifications + 1;
					    # if TRACES == 1
                printf("LambdaMin ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
					    # endif
				    }
			    }
				  else {
				    /* Borne sur LambdaMin */
            LambdaMin[Cnt] = X;
				    *NbModifications = *NbModifications + 1;
					  # if TRACES == 1
              printf("LambdaMin ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
				    # endif
            if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU ) ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_ET_MAX_CONNU;
			      else ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_CONNU;
				  }
			  }
		  }
		  else {
		    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {					 		
			    X = ( CBarreMin / CoeffDeVar ) + LambdaMin[Cnt];						
			    if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MAX_CONNU || ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {				
				    if ( X < LambdaMax[Cnt] ) {
              LambdaMax[Cnt] = X;
				      *NbModifications = *NbModifications + 1;
					    # if TRACES == 1
                printf("LambdaMax ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
				      # endif
				    }
				  }
				  else {
				    /* Borne sur LambdaMax */
            LambdaMax[Cnt] = X;
				    *NbModifications = *NbModifications + 1;
					  # if TRACES == 1
              printf("LambdaMax ameliore sur la contrainte %d: %e\n",Cnt,LambdaMax[Cnt]);
				    # endif
            if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_CONNU ) ConnaissanceDeLambda[Cnt] = LAMBDA_MIN_ET_MAX_CONNU;
			      else ConnaissanceDeLambda[Cnt] = LAMBDA_MAX_CONNU;					
				  }
			  }
		  }
      if ( ConnaissanceDeLambda[Cnt] == LAMBDA_MIN_ET_MAX_CONNU ) {
        if ( fabs( LambdaMin[Cnt] - LambdaMax[Cnt] ) < VALEUR_NULLE ) {	
	        Lambda[Cnt] = 0.5 * ( LambdaMin[Cnt] + LambdaMax[Cnt] );
		      LambdaMin[Cnt] = Lambda[Cnt];
		      LambdaMax[Cnt] = Lambda[Cnt];
          ConnaissanceDeLambda[Cnt] = LAMBDA_CONNU;
				  *NbModifications = *NbModifications + 1;
	      }
      }				
      ContrainteSuivante_2:
      il = Csui[il];
	  }
  }
}
						
return;

}

