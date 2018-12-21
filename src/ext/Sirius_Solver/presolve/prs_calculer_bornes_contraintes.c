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

   FONCTION: Calcul des bornes des contraintes.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define VALEUR_DE_BORNE_DEBILE_POUR_LES_CONTRAINTES 1.e+20 /*+15*/
# define ZERO_CONTRAINTE                             1.e-6
# define SEUIL_FORCING_CONTRAINTE                    1.e-7    	
# define SEUIL_PETIT_COEFFICIENT                     1.e-9    	

# define TRACES 0
   
/*----------------------------------------------------------------------------*/

void PRS_CalculerLesBornesDeToutesLesContraintes( PRESOLVE * Presolve, int * NbModifications )
{
int Cnt; char CalculDeForcingConstraints; int NbCntSuppr; int NbVarSuppr;
PROBLEME_PNE * Pne; char * ContrainteInactive;

*NbModifications = 0; /* Prend une valeur non nulle si forcing constraintes */

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

ContrainteInactive = Presolve->ContrainteInactive;

NbCntSuppr = 0;
NbVarSuppr = 0;

CalculDeForcingConstraints = OUI_PNE;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
  CalculDeForcingConstraints = NON_PNE;
}


for ( Cnt = 0 ; Cnt < Pne->NombreDeContraintesTrav ; Cnt++ ) {
  if ( ContrainteInactive[Cnt] != OUI_PNE ) {
    PRS_CalculerLesBornesDuneContrainte( Presolve, Cnt, CalculDeForcingConstraints,
					                               &NbCntSuppr, &NbVarSuppr );
  }
}

#if VERBOSE_PRS == 1 
  printf("-> Nombre de contraintes supprimees par forcing %d\n",NbCntSuppr);
  printf("-> Nombre de variable supprimees par forcing %d\n",NbVarSuppr);
  fflush(stdout);
#endif

*NbModifications = NbCntSuppr + NbVarSuppr;

return;
}

/*----------------------------------------------------------------------------*/

void PRS_CalculerLesBornesDuneContrainte( PRESOLVE * Presolve, int Cnt,  
                                          char CalculDeForcingConstraints,
					                                int * NbCntSuppr, int * NbVarSuppr )
{
int il; int ilMax; int Var; double Smin; double Smax; double S; 
char FixerLesVariablesSurMinContrainte; char FixerLesVariablesSurMaxContrainte;
int * Nuvar; double * A; double * BorneInfPourPresolve; double * BorneSupPourPresolve;
int * TypeDeBornePourPresolve; int * Mdeb; int * NbTerm; char * ContrainteInactive;
char * MinContrainteCalcule; char * MaxContrainteCalcule; double * ValeurDeXPourPresolve;
double * MinContrainte; double * MaxContrainte; char * SensContrainte; double * B;
PROBLEME_PNE * Pne; double ValeurDeX; char MinCntCalcule; char MaxCntCalcule;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

MinContrainteCalcule = Presolve->MinContrainteCalcule;    
MaxContrainteCalcule = Presolve->MaxContrainteCalcule;
MinContrainte = Presolve->MinContrainte;
MaxContrainte = Presolve->MaxContrainte;
ContrainteInactive = Presolve->ContrainteInactive;

MinCntCalcule = OUI_PNE;
MaxCntCalcule = OUI_PNE;

MinContrainteCalcule[Cnt] = MinCntCalcule;
MaxContrainteCalcule[Cnt] = MaxCntCalcule;

Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

Smin  = 0.;
Smax  = 0.;
il    = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
while ( il < ilMax ) {

  Var = Nuvar[il];

  if ( TypeDeBornePourPresolve[Var] == VARIABLE_NON_BORNEE ) {
    MinContrainteCalcule[Cnt] = NON_PNE;
    MaxContrainteCalcule[Cnt] = NON_PNE;
    MinContrainte[Cnt] = -LINFINI_PNE;
    MaxContrainte[Cnt] = LINFINI_PNE;		
    return;
  }
  else if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) {
    S = A[il] * ValeurDeXPourPresolve[Var];
    Smax += S;
    Smin += S;
  }
  else if ( A[il] != 0 ) {
    if ( A[il] > 0. ) {
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  MaxCntCalcule = NON_PNE;
				if ( MinCntCalcule == NON_PNE ) break;
			}
      else Smax += A[il] * BorneSupPourPresolve[Var];			
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  MinCntCalcule = NON_PNE;
				if ( MaxCntCalcule == NON_PNE ) break;
			}
      else Smin += A[il] * BorneInfPourPresolve[Var];			
    }
    else {
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
			  MaxCntCalcule = NON_PNE;
				if ( MinCntCalcule == NON_PNE ) break;
			}
      else Smax += A[il] * BorneInfPourPresolve[Var];		
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_BORNEE_INFERIEUREMENT ) {
			  MinCntCalcule = NON_PNE;
				if ( MaxCntCalcule == NON_PNE ) break;
			}
      else Smin += A[il] * BorneSupPourPresolve[Var];			
    }
  }
  il++;
}

MinContrainteCalcule[Cnt] = MinCntCalcule;
MaxContrainteCalcule[Cnt] = MaxCntCalcule;

MinContrainte[Cnt] = Smin;
MaxContrainte[Cnt] = Smax;

if ( MinContrainteCalcule[Cnt] == OUI_PNE ) { 
  if ( fabs( MinContrainte[Cnt] ) > VALEUR_DE_BORNE_DEBILE_POUR_LES_CONTRAINTES ) MinContrainteCalcule[Cnt] = NON_PNE;
} 

if ( MaxContrainteCalcule[Cnt] == OUI_PNE ) { 
  if ( fabs( MaxContrainte[Cnt] ) > VALEUR_DE_BORNE_DEBILE_POUR_LES_CONTRAINTES ) MaxContrainteCalcule[Cnt] = NON_PNE;
} 

if ( SensContrainte[Cnt] == '=' ) { 
  if ( MaxContrainteCalcule[Cnt] == OUI_PNE ) { 
    if ( MaxContrainte[Cnt] < B[Cnt] - ZERO_CONTRAINTE ) {
      # if TRACES == 1
		    printf("Probleme infaisable a cause de la contraintes %d MaxContrainte %e B %e sens %c\n",Cnt,MaxContrainte[Cnt],B[Cnt],SensContrainte[Cnt]);
		  # endif
      Pne->YaUneSolution = PROBLEME_INFAISABLE;			
      return;				
		}
  }
  if ( MinContrainteCalcule[Cnt] == OUI_PNE ) {
    if ( MinContrainte[Cnt] > B[Cnt] + ZERO_CONTRAINTE ) {
      # if TRACES == 1
		    printf("Probleme infaisable a cause de la contraintes %d MinContrainte %e B %e sens %c (MaxContrainte %e)\n",Cnt,MinContrainte[Cnt],B[Cnt],SensContrainte[Cnt],MaxContrainte[Cnt]);
		  # endif
      Pne->YaUneSolution = PROBLEME_INFAISABLE;			
      return;				
		}
	}	
}
else if ( SensContrainte[Cnt] == '<' ) {
  if ( MinContrainteCalcule[Cnt] == OUI_PNE ) {
    if ( MinContrainte[Cnt] > B[Cnt] + ZERO_CONTRAINTE ) {
      # if TRACES == 1
		    printf("Probleme infaisable a cause de la contraintes %d MinContrainte %e B %e sens %c\n",Cnt,MinContrainte[Cnt],B[Cnt],SensContrainte[Cnt]);
		  # endif
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
      return;				
		}
	}	
}
else if ( SensContrainte[Cnt] == '>' ) {
  if ( MaxContrainteCalcule[Cnt] == OUI_PNE ) { 
    if ( MaxContrainte[Cnt] < B[Cnt] - ZERO_CONTRAINTE ) {
      # if TRACES == 1
		    printf("Probleme infaisable a cause de la contraintes %d MaxContrainte %e B %e sens %c\n",Cnt,MaxContrainte[Cnt],B[Cnt],SensContrainte[Cnt]);
		  # endif
      Pne->YaUneSolution = PROBLEME_INFAISABLE;
      return;				
		}
  }
}

if ( CalculDeForcingConstraints == NON_PNE ) return;

/* Forcing constraints */
FixerLesVariablesSurMinContrainte = NON_PNE;
FixerLesVariablesSurMaxContrainte = NON_PNE;
if ( SensContrainte[Cnt] == '<' ) {
  if ( MinContrainteCalcule[Cnt] == OUI_PNE ) {
    if ( fabs( MinContrainte[Cnt] - B[Cnt] ) <= SEUIL_FORCING_CONTRAINTE ) {      
      # if TRACES == 1
        printf("Forcing contrainte inegalite %d \n",Cnt); 
		  # endif      
      FixerLesVariablesSurMinContrainte = OUI_PNE;   
    } 
  }
} 
else {
  /* Contrainte d'egalite */
  if ( MinContrainteCalcule[Cnt] == OUI_PNE ) {
    if ( fabs( MinContrainte[Cnt] - B[Cnt] ) <= SEUIL_FORCING_CONTRAINTE ) {
      # if TRACES == 1     
        printf("Forcing contrainte egalite %d MinContrainte %lf BTrav %lf\n",Cnt,MinContrainte[Cnt],B[Cnt]);  
		  # endif      
      
      FixerLesVariablesSurMinContrainte = OUI_PNE;  
    }  
  }
  if ( MaxContrainteCalcule[Cnt] == OUI_PNE ) {
    if ( fabs( MaxContrainte[Cnt] - B[Cnt] ) <= SEUIL_FORCING_CONTRAINTE ) {
      
      # if TRACES == 1     
        printf("Forcing contrainte egalite %d MaxContrainte %lf BTrav %lf\n",Cnt,MaxContrainte[Cnt],B[Cnt]);  
		  # endif      
      
      FixerLesVariablesSurMaxContrainte = OUI_PNE;
    }
  }
}

if ( FixerLesVariablesSurMinContrainte == NON_PNE && FixerLesVariablesSurMaxContrainte == NON_PNE ) return;

if ( Pne->NbForcingConstraints >= Pne-> NombreDeContraintesTrav ) return;
if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

/* Il faut verifier qu'il n'y a aucun coefficient nul (trop petit) parmi les variables non fixes sinon il
	 est trop risque de se prononcer a cause des imprecisions numeriques possibles */
  
il    = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
while ( il < ilMax ) {
  if ( TypeDeBornePourPresolve[Nuvar[il]] != VARIABLE_FIXE && A[il] != 0.0 ) {
    if ( fabs( A[il] ) < SEUIL_PETIT_COEFFICIENT ) return;
  }
  il++;
}

/* Fixation des variables */
il    = Mdeb[Cnt];
ilMax = il + NbTerm[Cnt];
if ( FixerLesVariablesSurMinContrainte == OUI_PNE ) {
  while ( il < ilMax ) {
    Var = Nuvar[il];
    if ( TypeDeBornePourPresolve[Var] != VARIABLE_FIXE && A[il] != 0. ) {
       	   	  
      if ( A[il] > 0. ) ValeurDeX = BorneInfPourPresolve[Var];
      else ValeurDeX = BorneSupPourPresolve[Var];
	                     						                
			# if TRACES == 1     
        printf("     forcing constrainte Var %d fixee a %e \n",Var,ValeurDeX);
			# endif

      PRS_FixerUneVariableAUneValeur( Presolve, Var, ValeurDeX ); 				
      *NbVarSuppr = *NbVarSuppr + 1;
										
    }              
    il++;
  }
}
else {
  while ( il < ilMax ) {
    Var = Nuvar[il];
    if ( TypeDeBornePourPresolve[Var] != VARIABLE_FIXE && A[il] != 0. ) {
	  	  
      if ( A[il] > 0. ) ValeurDeX = BorneSupPourPresolve[Var];
      else              ValeurDeX = BorneInfPourPresolve[Var];	            

			# if TRACES == 1     
        printf("     forcing contrainte Var %d fixee a %e \n",Var,ValeurDeX);  					
			# endif
          
      PRS_FixerUneVariableAUneValeur( Presolve, Var, ValeurDeX ); 									  
      *NbVarSuppr = *NbVarSuppr + 1;				
					
    }          
    il++;
  }
}

/* Desactivation de la contrainte: on peut la desactiver meme si elle est bornante
	 car on fixe toutes les variables de la contrainte */
PRS_DesactiverContrainte( Presolve, Cnt );
*NbCntSuppr = *NbCntSuppr + 1;

/* Mise a jour des infos pour le postsolve */
Pne->TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_FORCING_CONSTRAINT;
Pne->IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbForcingConstraints;
Pne->NombreDOperationsDePresolve++;	

Pne->NumeroDeLaForcingConstraint[Pne->NbForcingConstraints] = Pne->CorrespondanceCntPneCntEntree[Cnt];
Pne->NbForcingConstraints++;
	
return;
}

