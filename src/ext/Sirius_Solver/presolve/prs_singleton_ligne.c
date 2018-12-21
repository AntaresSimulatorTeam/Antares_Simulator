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

   FONCTION: On regarde si la resolution de certaines equations est 
             triviale. 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PRS_SingletonsSurLignes( PRESOLVE * Presolve, int * NbModifications )
{
int Cnt; int il ; int ilMax; int ilNonFix; int NbNonFix; int VarNonFix; int Var;
long double S; long double X; PROBLEME_PNE * Pne; int * Mdeb; int * NbTerm; int * Nuvar;
int TypeBrn; double * A; double * B; char * ContrainteInactive; char * SensContrainte;
int NombreDeContraintes; double * BorneInfPourPresolve; double * BorneSupPourPresolve;
int * TypeDeBornePourPresolve; double * ValeurDeXPourPresolve; int * TypeDeVariable;
int * ContrainteBornanteSuperieurement; int * ContrainteBornanteInferieurement;
int NombreDeContraintesSupprimees; int NombreDeVariablesFixees;
int NombreDeBornesModifiees; char BorneAmelioree; char * TypeDeValeurDeBorneInf;
char * TypeDeValeurDeBorneSup; int * NumeroDeLaContrainteSingleton; int * VariableDeLaContrainteSingleton;
char * TypeDOperationDePresolve; int * IndexDansLeTypeDOperationDePresolve;
double * SecondMembreDeLaContrainteSingleton; int * CorrespondanceCntPneCntEntree;

*NbModifications = 0;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
  return;
}

if ( Pne->NbLignesSingleton >= Pne->NombreDeContraintesTrav ) return;
if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

ContrainteInactive = Presolve->ContrainteInactive;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
TypeDeValeurDeBorneInf = Presolve->TypeDeValeurDeBorneInf;
TypeDeValeurDeBorneSup = Presolve->TypeDeValeurDeBorneSup;

ContrainteBornanteSuperieurement = Presolve->ContrainteBornanteSuperieurement;
ContrainteBornanteInferieurement = Presolve->ContrainteBornanteInferieurement;

CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;
TypeDeVariable = Pne->TypeDeVariableTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
B = Pne->BTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

TypeDOperationDePresolve = Pne->TypeDOperationDePresolve;
IndexDansLeTypeDOperationDePresolve = Pne->IndexDansLeTypeDOperationDePresolve;			
NumeroDeLaContrainteSingleton = Pne->NumeroDeLaContrainteSingleton;
VariableDeLaContrainteSingleton = Pne->VariableDeLaContrainteSingleton;
SecondMembreDeLaContrainteSingleton = Pne->SecondMembreDeLaContrainteSingleton;

NombreDeContraintesSupprimees = 0;
NombreDeVariablesFixees = 0;
NombreDeBornesModifiees = 0;
 
/* On regarde si on peut resoudre certaines contraintes "a la main" */

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {

  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;
  if ( Pne->NbLignesSingleton >= NombreDeContraintes ) return;
  if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

  S        = 0.;
  NbNonFix = 0;  
  ilNonFix = -1;
	VarNonFix = -1;	
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];	
  while ( il < ilMax ) {
    if ( A[il] != 0.0 ) {
      Var = Nuvar[il];
      if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) {
        /* La variable est fixee */
        S += A[il] * ValeurDeXPourPresolve[Var];
			}
			else {		
        NbNonFix++; 
        VarNonFix = Var;
        ilNonFix  = il;
				if ( NbNonFix > 1 ) break;
      }
		}
    il++;		 
  }

  if ( NbNonFix != 1 ) continue;
	if ( fabs( A[ilNonFix] ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) continue;
 
  /* Si la contrainte n'a qu'une seule variable on peut resoudre directement */
	TypeBrn = TypeDeBornePourPresolve[VarNonFix];  
  X = ( (long double) B[Cnt] - S ) / (long double) A[ilNonFix];
	
  if ( SensContrainte[Cnt] == '=' ) {	
    /* Test d'admissibilite */	
    if ( TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT ) { 
      if ( X < BorneInfPourPresolve[VarNonFix] - SEUIL_DADMISSIBILITE ) {
				# if VERBOSE_PRS == 1
          printf("*** Ligne singleton -> La contrainte %d ne peut pas etre satisfaite\n",Cnt);
          printf("                       Variable %d BorneInfPourPresolve %e  X %e \n",
					                               VarNonFix,BorneInfPourPresolve[VarNonFix],(double) X);					
        # endif					
				Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
      }
    }
    else if ( TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT ) { 
      if ( X > BorneSupPourPresolve[VarNonFix] + SEUIL_DADMISSIBILITE ) { 
				# if VERBOSE_PRS == 1
          printf("*** Ligne singleton -> La contrainte %d ne peut pas etre satisfaite\n",Cnt);
          printf("                       Variable %d X %e BorneSupPourPresolve %e\n",
					                               VarNonFix,(double) X,BorneSupPourPresolve[VarNonFix]);					
        # endif				
        Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
      }
    }		
    else if ( TypeBrn == VARIABLE_BORNEE_DES_DEUX_COTES ) { 
      if ( X < BorneInfPourPresolve[VarNonFix] - SEUIL_DADMISSIBILITE || X > BorneSupPourPresolve[VarNonFix] + SEUIL_DADMISSIBILITE ) { 
				# if VERBOSE_PRS == 1
          printf("*** Ligne singleton -> La contrainte %d ne peut pas etre satisfaite\n",Cnt);
          printf("                       Variable %d BorneInfPourPresolve %e  X %e BorneSupPourPresolve %e\n",
					                               VarNonFix,BorneInfPourPresolve[VarNonFix],(double) X,BorneSupPourPresolve[VarNonFix]);
        # endif				
        Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
      }
    }	
		if ( TypeDeVariable[VarNonFix] == ENTIER ) {		
		  if ( fabs( X ) > MARGE_DINFAISABILITE && fabs( 1 - X ) > MARGE_DINFAISABILITE ) {			
			  Pne->YaUneSolution = PROBLEME_INFAISABLE; return;
			}
    }

		# if TRACES == 1
		  printf("Ligne singleton fixation de la variable %d a %e grace a la contrainte d'egalite %d\n",VarNonFix,(double) X,Cnt);			
		# endif

    /* Infos pour le postsolve */
    TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_LIGNE_SINGLETON;
    IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbLignesSingleton;
    Pne->NombreDOperationsDePresolve++;
    NumeroDeLaContrainteSingleton[Pne->NbLignesSingleton] = CorrespondanceCntPneCntEntree[Cnt];		
    VariableDeLaContrainteSingleton[Pne->NbLignesSingleton] = VarNonFix;
    SecondMembreDeLaContrainteSingleton[Pne->NbLignesSingleton] = X;		
    Pne->NbLignesSingleton++;
		/*                         */
							
    PRS_FixerUneVariableAUneValeur( Presolve, VarNonFix, (double) X );
    NombreDeVariablesFixees++; 
		
    /* On peut desactiver meme si elle est bornante car toutes les variables de la contrainte
		   deviennent fixes */
    PRS_DesactiverContrainte( Presolve, Cnt );		
    NombreDeContraintesSupprimees++;
		
  }
  else { /* la contraintes est forcement < */	
    if ( A[ilNonFix] > 0. ) {
	    /* X est une borne sup candidate */			
      PRS_MettreAJourLesBornesDUneVariable( Presolve, VarNonFix, NON_PNE, -LINFINI_PNE, -1, 
																		        OUI_PNE, (double) X, Cnt, &BorneAmelioree );	
	    if ( BorneAmelioree == OUI_PNE ) {
			  NombreDeBornesModifiees++;
        /* Infos pour le postsolve */
        TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_LIGNE_SINGLETON;
        IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbLignesSingleton;
        Pne->NombreDOperationsDePresolve++;				
        NumeroDeLaContrainteSingleton[Pne->NbLignesSingleton] = CorrespondanceCntPneCntEntree[Cnt];
        VariableDeLaContrainteSingleton[Pne->NbLignesSingleton] = VarNonFix;
        SecondMembreDeLaContrainteSingleton[Pne->NbLignesSingleton] = X;		
        Pne->NbLignesSingleton++;
		    /*                         */				
			}
			/* Soit on a mis a jour la borne et alors on n'a plus besoin de la contrainte. Soit on ne l'a pas
			   mise a jour et on n'a plus non plus besoin de la contrainte car la borne de la variable etait plus
				 restrictive que la contrainte */
      PRS_DesactiverContrainte( Presolve, Cnt );		
      NombreDeContraintesSupprimees++;			
    }
    else {
		  /* X est une borne inf candidate */
      PRS_MettreAJourLesBornesDUneVariable( Presolve, VarNonFix, OUI_PNE, (double) X, Cnt,
																		        NON_PNE, LINFINI_PNE, -1, &BorneAmelioree );
	    if ( BorneAmelioree == OUI_PNE ) {
			  NombreDeBornesModifiees++;
        /* Infos pour le postsolve */
        TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUPPRESSION_LIGNE_SINGLETON;
        IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbLignesSingleton;
        Pne->NombreDOperationsDePresolve++;
				
        NumeroDeLaContrainteSingleton[Pne->NbLignesSingleton] = CorrespondanceCntPneCntEntree[Cnt];
        VariableDeLaContrainteSingleton[Pne->NbLignesSingleton] = VarNonFix;
        SecondMembreDeLaContrainteSingleton[Pne->NbLignesSingleton] = X;						
        Pne->NbLignesSingleton++;						
		    /*                         */				
			}
			/* Soit on a mis a jour la borne et alors on n'a plus besoin de la contrainte. Soit on ne l'a pas
			   mise a jour et on n'a plus non plus besoin de la contrainte car la borne de la variable etait plus
				 restrictive que la contrainte */
      PRS_DesactiverContrainte( Presolve, Cnt );		
      NombreDeContraintesSupprimees++;				 			
    }
  }	
}   

#if VERBOSE_PRS
  printf("-> Lignes singleton: contraintes supprimees %d / bornes modifiees %d / variables fixees %d / total %d\n",	
	           NombreDeContraintesSupprimees,NombreDeBornesModifiees,NombreDeVariablesFixees,						 
						 NombreDeContraintesSupprimees+NombreDeBornesModifiees+NombreDeVariablesFixees); 
#endif   

*NbModifications = NombreDeContraintesSupprimees + NombreDeVariablesFixees + NombreDeBornesModifiees;
 
return;
}

