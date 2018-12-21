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

   FONCTION: On essai de voir si les contraintes d'admissibilite duale
	           peuvent conduire a ameliorer des variables duales.
						 On essaie de voir s'il est possible de fixer des variables
						 sur la base des couts reduits.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"

# include "pne_define.h"

# include "prs_define.h"

# define ZERO_CBARRE  1.e-7
# define VALEUR_NULLE 1.e-8
# define MX_ITER 10
 
# define TRACES 0
# define TRACES_VARIABLES_FAIBLEMENT_DOMINEES 0

# define ELIMINATION_DES_COLONNES_FAIBLEMENT_DOMINEES NON_PNE
										 
/*----------------------------------------------------------------------------*/
/*              On regarde si on peut fixer des variables entieres ou
                resserer des bornes sur les variables                         */                     

void PRS_VariablesDualesEtCoutsReduits( PRESOLVE * Presolve, int * BorneAmelioree )
{
int Var; int NbCycles; PROBLEME_PNE * Pne; int * TypeDeBornePourPresolve;
int NombreDeVariables; int NbModifications; int NbSv;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

*BorneAmelioree = NON_PNE;  
NbCycles = 0;
NbModifications = 0;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
  return;
}

/* Scanner les colonnes pour resserer les bornes sur la variable qui correspond
   a la colonne */

while ( NbCycles < MX_ITER ) {
  NbCycles++;					
  NbSv = NbModifications;
  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {		
    if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;		
    PRS_AmeliorerOuCalculerCoutsReduits( Presolve, Var, &NbModifications );		
	  if ( Pne->YaUneSolution == PROBLEME_INFAISABLE ) return;
  }
	if ( NbModifications == NbSv ) break;				
}

#if VERBOSE_PRS
  printf("-> Examen des couts reduits. Nombre de bornes amelioree ou de variables fixees %d\n",NbModifications);
#endif

if ( NbModifications > 0 ) *BorneAmelioree = OUI_PNE;

return;
}

/*----------------------------------------------------------------------------*/

void PRS_AmeliorerOuCalculerCoutsReduits( PRESOLVE * Presolve,
				                                  int   Var, 
                                          int * NbModifications )
{
int il; int Cnt; double CoeffDeVar; PROBLEME_PNE * Pne; double * LambdaMin; double * LambdaMax;
double * Lambda; int * Cdeb; int * Csui; int * NumContrainte; double * CoutLineaire;
double CBarre; char * ContrainteInactive; double CBarreMin; double CBarreMax; 
int NbTCol; int CntInvalide; double CoeffDeVarCntInvalide; char * ConnaissanceDeLambda;
double * A; char TypeBrn; double * ValeurDeXPourPresolve; double * BorneInfPourPresolve;
int * TypeDeVariableNative; double * BorneSupPourPresolve; int * TypeDeBornePourPresolve;
int * TypeDeBorneNative; double * BorneInfNative; double * BorneSupNative; char * SensContrainte;
char * ConserverLaBorneInfDuPresolve; char * ConserverLaBorneSupDuPresolve;
double X; double SommeDesAiFoisLambda; double MinSommeDesAiFoisLambda; double MaxSommeDesAiFoisLambda;
char SommeDesAiFoisLambdaValide; char MinSommeDesAiFoisLambdaValide; char MaxSommeDesAiFoisLambdaValide;
int NbTSommeDesAiFoisLambda; int NbTMinSommeDesAiFoisLambda; int NbTMaxSommeDesAiFoisLambda;
int CntMaxInvalide; double CoeffDeVarCntMaxInvalide; int CntMinInvalide; double CoeffDeVarCntMinInvalide;
char ConnaissanceDeLambdaCnt; char * TypeDeValeurDeBorneInf; char * TypeDeValeurDeBorneSup;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
SensContrainte = Pne->SensContrainteTrav;
A = Pne->ATrav;

CoutLineaire = Pne->LTrav;
TypeDeVariableNative = Pne->TypeDeVariableTrav;
BorneInfNative = Pne->UminTrav;
BorneSupNative = Pne->UmaxTrav;
TypeDeBorneNative = Pne->TypeDeBorneTrav;

LambdaMin = Presolve->LambdaMin;
LambdaMax = Presolve->LambdaMax;
Lambda = Presolve->Lambda;
ConnaissanceDeLambda = Presolve->ConnaissanceDeLambda;
ContrainteInactive = Presolve->ContrainteInactive;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
TypeDeValeurDeBorneSup = Presolve->TypeDeValeurDeBorneSup;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;

/* Produit scalaire du vecteur des variables duales et de la colonne de la variable */
SommeDesAiFoisLambda = 0.;
SommeDesAiFoisLambdaValide = OUI_PNE;
MinSommeDesAiFoisLambda = 0.;
MinSommeDesAiFoisLambdaValide = OUI_PNE;
MaxSommeDesAiFoisLambda = 0.;
MaxSommeDesAiFoisLambdaValide = OUI_PNE;

NbTCol = 0;
NbTSommeDesAiFoisLambda = 0; 
NbTMinSommeDesAiFoisLambda = 0;
NbTMaxSommeDesAiFoisLambda = 0;

CntInvalide = -1;
CoeffDeVarCntInvalide = 0.;
CntMaxInvalide = -1;
CoeffDeVarCntMaxInvalide = 0.;
CntMinInvalide = -1;
CoeffDeVarCntMinInvalide = 0.;

CoeffDeVar = 1.; /* Pour eviter les warning de compilation */
il = Cdeb[Var];
while ( il >= 0 ) {

  Cnt = NumContrainte[il];
  if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante;

  CoeffDeVar = A[il];
		
	if ( CoeffDeVar != 0.0 ) {
		NbTCol++;
		ConnaissanceDeLambdaCnt = ConnaissanceDeLambda[Cnt];
	  if ( ConnaissanceDeLambdaCnt == LAMBDA_CONNU ) {
      X = Lambda[Cnt] * CoeffDeVar;
      SommeDesAiFoisLambda += X;
      MinSommeDesAiFoisLambda += X;
      MaxSommeDesAiFoisLambda += X;
      NbTSommeDesAiFoisLambda++;
      NbTMinSommeDesAiFoisLambda++;			
      NbTMaxSommeDesAiFoisLambda++;			
	  }
	  else {
			CntInvalide = Cnt;
			CoeffDeVarCntInvalide = CoeffDeVar;		
			SommeDesAiFoisLambdaValide = NON_PNE;
  	  if ( CoeffDeVar > 0.0 ) {
			  /* MaxSommeDesAiFoisLambda */
		    if ( ConnaissanceDeLambdaCnt == LAMBDA_MAX_CONNU || ConnaissanceDeLambdaCnt == LAMBDA_MIN_ET_MAX_CONNU ) {
				  MaxSommeDesAiFoisLambda += CoeffDeVar * LambdaMax[Cnt];
					NbTMaxSommeDesAiFoisLambda++;
				}
				else {
				  MaxSommeDesAiFoisLambdaValide = NON_PNE;
          CntMaxInvalide = Cnt;
          CoeffDeVarCntMaxInvalide = CoeffDeVar;					
				}
			  /* MinSommeDesAiFoisLambda */				
		    if ( ConnaissanceDeLambdaCnt == LAMBDA_MIN_CONNU || ConnaissanceDeLambdaCnt == LAMBDA_MIN_ET_MAX_CONNU ) {
				  MinSommeDesAiFoisLambda += CoeffDeVar * LambdaMin[Cnt];
          NbTMinSommeDesAiFoisLambda++;								
        }
        else {
				  MinSommeDesAiFoisLambdaValide = NON_PNE;
          CntMinInvalide = Cnt;
          CoeffDeVarCntMinInvalide = CoeffDeVar;
				}				
			}
			else {
        /* CoeffDeVar < 0 */
			  /* MaxSommeDesAiFoisLambda */			
		    if ( ConnaissanceDeLambdaCnt == LAMBDA_MIN_CONNU || ConnaissanceDeLambdaCnt == LAMBDA_MIN_ET_MAX_CONNU ) {
				  MaxSommeDesAiFoisLambda += CoeffDeVar * LambdaMin[Cnt];
					NbTMaxSommeDesAiFoisLambda++;
				}
				else {
				  MaxSommeDesAiFoisLambdaValide = NON_PNE;
          CntMaxInvalide = Cnt;
          CoeffDeVarCntMaxInvalide = CoeffDeVar;									
        }
			  /* MinSommeDesAiFoisLambda */								
		    if ( ConnaissanceDeLambdaCnt == LAMBDA_MAX_CONNU || ConnaissanceDeLambdaCnt == LAMBDA_MIN_ET_MAX_CONNU ) {
				  MinSommeDesAiFoisLambda += CoeffDeVar * LambdaMax[Cnt];
          NbTMinSommeDesAiFoisLambda++;								
				}
        else {
				  MinSommeDesAiFoisLambdaValide = NON_PNE;
					CntMinInvalide = Cnt;
          CoeffDeVarCntMinInvalide = CoeffDeVar;
        }				
			}
	  }
	}		
	
  ContrainteSuivante:
	if ( SommeDesAiFoisLambdaValide == NON_PNE && MinSommeDesAiFoisLambdaValide == NON_PNE && MaxSommeDesAiFoisLambdaValide == NON_PNE ) {
	  if ( ( NbTCol - NbTSommeDesAiFoisLambda != 1 ) && ( NbTCol - NbTMinSommeDesAiFoisLambda != 1 ) && ( NbTCol - NbTMaxSommeDesAiFoisLambda != 1 )  ) break;
  }	
  il = Csui[il];
}

if ( NbTCol <= 0 ) return;

/* Examen des couts reduits */
if ( SommeDesAiFoisLambdaValide == NON_PNE ) {	
  if ( MinSommeDesAiFoisLambdaValide == OUI_PNE && MaxSommeDesAiFoisLambdaValide == OUI_PNE ) {
    if ( fabs( MinSommeDesAiFoisLambda - MaxSommeDesAiFoisLambda ) < VALEUR_NULLE ) {
		  SommeDesAiFoisLambdaValide = OUI_PNE;
      SommeDesAiFoisLambda = 0.5 * ( MinSommeDesAiFoisLambda + MaxSommeDesAiFoisLambda );
			MinSommeDesAiFoisLambda = SommeDesAiFoisLambda;
			MaxSommeDesAiFoisLambda = SommeDesAiFoisLambda;			
		}
	}
}
X = CoutLineaire[Var];
CBarre = X - SommeDesAiFoisLambda;
CBarreMin = X - MaxSommeDesAiFoisLambda;
CBarreMax = X - MinSommeDesAiFoisLambda;

/* Determination du type de borne le mieux adapte pour contraindre le signe du cout reduit */
TypeBrn = PRS_ChoisirLaBorneLaPlusContraignantePourLesVariablesDuales( 
                      TypeDeBorneNative[Var], TypeDeBornePourPresolve[Var],
										  BorneInfPourPresolve[Var], BorneInfNative[Var],
										  BorneSupPourPresolve[Var], BorneSupNative[Var],
										  ConserverLaBorneInfDuPresolve[Var], ConserverLaBorneSupDuPresolve[Var],
										  TypeDeValeurDeBorneInf[Var], TypeDeValeurDeBorneSup[Var] );
																					
/* Amelioration des bornes sur les variables duales */
PRS_AmeliorerBornesDesVariablesDuales( Presolve, Var, TypeBrn, MinSommeDesAiFoisLambda, MinSommeDesAiFoisLambdaValide,
																			 MaxSommeDesAiFoisLambda, MaxSommeDesAiFoisLambdaValide, NbModifications );

if ( SommeDesAiFoisLambdaValide == OUI_PNE ) {	
  if ( CBarre > ZERO_CBARRE ) {
    /* Le cout reduit est toujours positif: la variable est sur borne inf */
    if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT || TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) {
		  # if TRACES == 1
        printf("-> CBarreValide %e: variable %d fixee a %e UminTrav = %e \n",CBarre,Var,BorneInfPourPresolve[Var],Pne->UminTrav[Var]); 
      # endif			
      *NbModifications = *NbModifications + 1;
			PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] ); return;   			
		}
    else {
      # if TRACES == 1
        printf("-> variable non bornee %d cout reduit non nul %e cout lineaire %e. Le probleme n'a pas de solution.\n",Var,CoutLineaire[Var],CBarre); 
      # endif				  
      Pne->YaUneSolution = PROBLEME_INFAISABLE;			
		}				
		return;
	}
  if ( CBarre < -ZERO_CBARRE ) {
    /* Le cout reduit est toujours negatif: la variable est sur borne sup */
    if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT || TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) {			
      # if TRACES == 1
        printf("-> CBarreValide %e: variable %d fixee a %e UmaxTrav = %e\n",CBarre,Var,BorneSupPourPresolve[Var],Pne->UmaxTrav[Var]); 
      # endif			
      *NbModifications = *NbModifications + 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] ); return;   			
		}
    else {
      # if TRACES == 1
        printf("-> variable non bornee %d cout reduit non nul %e cout lineaire %e. Le probleme n'a pas de solution.\n",Var,CoutLineaire[Var],CBarre); 
      # endif				  
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
		}				
		return; 
  }

  # if ELIMINATION_DES_COLONNES_FAIBLEMENT_DOMINEES == OUI_PNE	
	  else {
	    /* Cout reduit nul */		
			/* Attention pas de variable faiblement dominee si elle est bornee des 2 cotes */
	    if ( NbTCol > 1 ) {
	      /* Weakly dominated column */
	      if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
          /* On fixe la variable sur sa borne inf sauf si c'est une colonne singleton */	
		      # if TRACES_VARIABLES_FAIBLEMENT_DOMINEES == 1
            printf("-> 1- Variable faiblement dominee %d CBarre % e fixee a sa borne inf\n",Var,CBarre);
            printf("      La variable est de type VARIABLE_BORNEE_INFERIEUREMENT\n");					
          # endif								
          *NbModifications = *NbModifications + 1;
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] ); return;   							  				  
        }				
	      else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
          /* On fixe la variable sur sa borne sup sauf si c'est une colonne singleton */			
		      # if TRACES_VARIABLES_FAIBLEMENT_DOMINEES == 1
            printf("-> 2- Variable faiblement dominee %d CBarre % e fixee a sa borne sup\n",Var,CBarre);
            printf("      La variable est de type VARIABLE_BORNEE_SUPERIEUREMENT\n");					
          # endif				
          *NbModifications = *NbModifications + 1;
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] ); return;
			  }				    
	  	}
	  }
	# endif
	
}

if ( MaxSommeDesAiFoisLambdaValide == OUI_PNE ) {
  /* On dispose donc de CBarreMin */
  
	# if TRACES == 1
	  if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) printf("Variable %d CBarreMin %e VARIABLE_BORNEE_INFERIEUREMENT NbTCol %d\n",Var,CBarreMin,NbTCol);
	  if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) printf("Variable %d CBarreMin %e VARIABLE_BORNEE_SUPERIEUREMENT NbTCol %d\n",Var,CBarreMin,NbTCol);
	  if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("Variable %d CBarreMin %e VARIABLE_BORNEE_DES_DEUX_COTES NbTCol %d\n",Var,CBarreMin,NbTCol);
	  if ( TypeBrn == VARIABLE_NON_BORNEE ) printf("Variable %d CBarreMin %e VARIABLE_NON_BORNEE NbTCol %d\n",Var,CBarreMin,NbTCol);
	# endif
	
  if ( CBarreMin > ZERO_CBARRE ) {
    /* Le cout reduit est toujours positif: la variable est sur borne inf */
    if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT || TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) {
		  # if TRACES == 1
        printf("-> CBarreMinValide %e: variable %d fixee a %e UminTrav = %e\n",CBarreMin,Var,BorneInfPourPresolve[Var],Pne->UminTrav[Var]); 
      # endif			
      *NbModifications = *NbModifications + 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] ); return;   			      
		}
    else {
      # if TRACES == 1
        if ( TypeBrn == VARIABLE_NON_BORNEE ) {
				  printf("-> variable non bornee %d cout reduit min %e cout lineaire %e. Le probleme n'a pas de solution\n",
					           Var,CBarreMin,CoutLineaire[Var]); 
        }
				else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
				  printf("-> variable non bornee %d cout reduit min %e cout lineaire %e. Le probleme n'a pas de solution\n",
					           Var,CoutLineaire[Var],CBarreMin);										 
				}
      # endif			
      Pne->YaUneSolution = PROBLEME_INFAISABLE;			
			return;
		}				
	}	
  else if ( fabs( CBarreMin ) < VALEUR_NULLE ) {	
	  if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
      # if TRACES == 1
		    printf("->Forcing dual constraint grace a CBarreMin nul\n");
			# endif							
			/* On doit mettre au max */
			il = Cdeb[Var];
			while ( il >= 0 ) {
			  Cnt = NumContrainte[il];
        if ( ContrainteInactive[Cnt] == NON_PNE && A[il] != 0.0 ) {
          if ( ConnaissanceDeLambda[Cnt] != LAMBDA_CONNU ) {							
				    if ( A[il] > 0.0 ) { Lambda[Cnt] = LambdaMax[Cnt]; LambdaMin[Cnt] = Lambda[Cnt]; }				
					  else               { Lambda[Cnt] = LambdaMin[Cnt]; LambdaMax[Cnt] = Lambda[Cnt]; }						
            ConnaissanceDeLambda[Cnt] = LAMBDA_CONNU;								
            *NbModifications = *NbModifications + 1;
					}
				}
			  il = Csui[il];
			}
		}
		
    # if ELIMINATION_DES_COLONNES_FAIBLEMENT_DOMINEES == OUI_PNE	
	    /* Weakly dominated column */
	    /* Attention pas de variable faiblement dominee si elle est bornee des 2 cotes */		
	    if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
        /* On fixe la variable sur sa borne inf sauf si c'est une colonne singleton */	
			  if ( NbTCol > 1 ) {
		      # if TRACES_VARIABLES_FAIBLEMENT_DOMINEES == 1
            printf("-> 1- Variable faiblement dominee %d CBarreMin %e fixee a sa borne inf\n",Var,CBarreMin);
            if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) printf("      Variable de type VARIABLE_BORNEE_INFERIEUREMENT\n");
            if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("      Variable de type VARIABLE_BORNEE_DES_DEUX_COTES\n");
          # endif				 					
          *NbModifications = *NbModifications + 1;
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneInfPourPresolve[Var] ); return;   			      				
			  }				  
      }		
	  # endif
		
	}	
}

if ( MinSommeDesAiFoisLambdaValide == OUI_PNE ) {
  /* On dispose donc de CBarreMax */
	
	# if TRACES == 1
	  if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) printf("Variable %d CBarreMax %e VARIABLE_BORNEE_INFERIEUREMENT NbTCol %d\n",Var,CBarreMax,NbTCol);
	  if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) printf("Variable %d CBarreMax %e VARIABLE_BORNEE_SUPERIEUREMENT NbTCol %d\n",Var,CBarreMax,NbTCol);
	  if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("Variable %d CBarreMax %e VARIABLE_BORNEE_DES_DEUX_COTES NbTCol %d\n",Var,CBarreMax,NbTCol);
	  if ( TypeBrn == VARIABLE_NON_BORNEE ) printf("Variable %d CBarreMax %e VARIABLE_NON_BORNEE NbTCol %d\n",Var,CBarreMax,NbTCol);
	# endif
	
  if ( CBarreMax < -ZERO_CBARRE ) {
    /* Le cout reduit est toujours negatif: la variable est sur borne sup */
    if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT || TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) {				 
		  # if TRACES == 1
        printf("->CBarreMaxValide  %e: variable %d fixee a %e UmaxTrav = %e\n",CBarreMax,Var,BorneSupPourPresolve[Var],Pne->UmaxTrav[Var]); 
      # endif			
      *NbModifications = *NbModifications + 1;
      PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] ); return;			
		}
    else {
      # if TRACES == 1
        printf("-> variable non bornee %d cout reduit max %e cout lineaire %e. Le probleme n'a pas de solution\n",
				           Var,CBarreMax,CoutLineaire[Var]);								 
      # endif				  
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
			return;
		}
	}   
  else if ( fabs( CBarreMax ) < VALEUR_NULLE ) {	
	  if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {
		  # if TRACES == 1
		    printf("-> Forcing dual constraint grace a CBarreMax nul\n");
			# endif							
			/* On doit mettre ua au min */
			il = Cdeb[Var];
			while ( il >= 0 ) {
			  Cnt = NumContrainte[il];
        if ( ContrainteInactive[Cnt] == NON_PNE && A[il] != 0.0 ) {
					if ( ConnaissanceDeLambda[Cnt] != LAMBDA_CONNU ) {
				    if ( A[il] > 0.0 ) { Lambda[Cnt] = LambdaMin[Cnt]; LambdaMax[Cnt] = Lambda[Cnt]; }
					  else               { Lambda[Cnt] = LambdaMax[Cnt]; LambdaMin[Cnt] = Lambda[Cnt]; }						
            ConnaissanceDeLambda[Cnt] = LAMBDA_CONNU;				    
            *NbModifications = *NbModifications + 1;
					}					
				}
				il = Csui[il];
			}
		}

    # if ELIMINATION_DES_COLONNES_FAIBLEMENT_DOMINEES == OUI_PNE			
      /* Variable dite "weakly dominated ?" */
	    /* Attention pas de variable faiblement dominee si elle est bornee des 2 cotes */			
	    if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {
        /* On fixe la variable sur sa borne sup sauf si c'est une colonne singleton */			
			  if ( NbTCol > 1 ) {
		      # if TRACES_VARIABLES_FAIBLEMENT_DOMINEES == 1
            printf("-> 2- Variable faiblement dominee %d CBarreMax %e fixee a sa borne sup\n",Var,CBarreMax);
            if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) printf("      Variable de type VARIABLE_BORNEE_SUPERIEUREMENT\n");
            if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) printf("      Variable de type VARIABLE_BORNEE_DES_DEUX_COTES\n");					
          # endif				
          *NbModifications = *NbModifications + 1;
          PRS_FixerUneVariableAUneValeur( Presolve, Var, BorneSupPourPresolve[Var] ); return;
			  }				    
      }
		# endif
		
	}	
}

/* Tentative d'amelioration des bornes sur les variables duales */
																			 
if ( ( CntInvalide >= 0 && CoeffDeVarCntInvalide != 0.0 ) ||
     ( CntMaxInvalide >= 0 && CoeffDeVarCntMaxInvalide != 0.0 ) ||
		 ( CntMinInvalide >= 0 && CoeffDeVarCntMinInvalide != 0.0 ) ) {		 
  /* On essaie de calculer des variables duales ou des bornes a l'aide la partie disponible de CBarre */																																			
  if ( SommeDesAiFoisLambdaValide == NON_PNE && CntInvalide >= 0 && CoeffDeVarCntInvalide != 0.0 ) {	
	  if ( NbTCol - NbTSommeDesAiFoisLambda == 1 ) {
		  /* On dispose de CBarre sauf pour une variable duale */	  
      if ( TypeBrn == VARIABLE_NON_BORNEE ) {			
        # if TRACES == 1
		      printf("On peut calculer un lambda grace a la variable non bornee %d pour la contrainte %d\n",Var,CntInvalide);
        # endif				
        PRS_MajVariableDuale( Presolve, CntInvalide, CBarre, CoeffDeVarCntInvalide, '=', NbModifications );  						
	    }
			else if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {			
        # if TRACES == 1
          printf("On peut calculer un lambda min ou max grace a la variable bornee inferieurement %d pour la contrainte %d\n",Var,CntInvalide);
        # endif				
				/* On doit avoir CBarre >= 0 */
        PRS_MajVariableDuale( Presolve, CntInvalide, CBarre, CoeffDeVarCntInvalide, '>', NbModifications );  
			}
	    else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {			
        # if TRACES == 1
          printf("On peut calculer un lambda min ou max grace a la variable bornee superieurement %d pour la contrainte %d\n",Var,CntInvalide);
        # endif
				/* On doit avoir CBarre <= 0 */
        PRS_MajVariableDuale( Presolve, CntInvalide, CBarre, CoeffDeVarCntInvalide, '<', NbModifications );  
		  }		 
	  }
	}

	if ( MinSommeDesAiFoisLambdaValide == NON_PNE && CntMinInvalide >= 0 && CoeffDeVarCntMinInvalide != 0.0 ) {
    if ( NbTCol - NbTMinSommeDesAiFoisLambda == 1 ) {		 
		  /* On dispose de CBarreMax sauf pour une variable duale */	  
	    if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) {			
        # if TRACES == 1
          printf("On peut calculer un lambda min ou max grace a la variable bornee inferieurement %d pour la contrainte %d\n",Var,CntInvalide);
        # endif
	      /* On doit avoir CBarre >= 0 */
        PRS_MajVariableDuale( Presolve, CntMinInvalide, CBarreMax, CoeffDeVarCntMinInvalide, '>', NbModifications );				
				return;       	
		  }
	  }
	}

 	if ( MaxSommeDesAiFoisLambdaValide == NON_PNE && CntMaxInvalide >= 0 && CoeffDeVarCntMaxInvalide != 0.0 ) {
	  if ( NbTCol - NbTMaxSommeDesAiFoisLambda == 1 ) {	
		  /* On dispose de CBarreMin sauf pour une variable duale */	  
	    if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) {			
        # if TRACES == 1
          printf("On peut calculer un lambda min ou max grace a la variable bornee superieurement %d pour la contrainte %d\n",Var,CntInvalide);
        # endif				
	      /* On doit avoir CBarre <= 0 */
        PRS_MajVariableDuale( Presolve, CntMaxInvalide, CBarreMin, CoeffDeVarCntMaxInvalide, '<', NbModifications );				
				return;						
		  }
	  }	
  }
}

return;

}

