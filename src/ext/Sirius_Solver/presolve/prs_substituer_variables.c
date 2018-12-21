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

   FONCTION: On tente de faire des substitutions de variables grace aux
             contraintes d'égalite ne comprenant que 2 termes.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES 0

# define ATILDE_MIN 1.e-5
# define ATILDE_MAX 1.e+5

void PRS_CalculerLesNouvellesBornesDeLaVariableRestante( PRESOLVE * , int ,int , double , double );
																												 
/*----------------------------------------------------------------------------*/

void PRS_SubstituerVariables( PRESOLVE * Presolve, int * NbModifications )
{		      
int Cnt; int Var1; int Var2; double CoeffDeVar1; double CoeffDeVar2; int il;
int ilMax; char   SubstitutionPossible; int il1; int il2; PROBLEME_PNE * Pne;
int NombreDeContraintes; char * ContrainteInactive; char * SensContrainte;
int * Mdeb; int * NbTerm; int * Nuvar; int * TypeDeBornePourPresolve;
int * TypeDeVariableNative; double * A; double * BorneInfPourPresolve;
double * BorneSupPourPresolve; double * B; double BTilde; double ATilde;
double BCnt; char * ConserverLaBorneSupDuPresolve; char * ConserverLaBorneInfDuPresolve;
double S; double * ValeurDeXPourPresolve; double X; double * CoutLineaire;
int * NumeroDesVariablesSubstituees; double * CoutDesVariablesSubstituees;  
double * ValeurDeLaConstanteDeSubstitution; int * IndiceDebutVecteurDeSubstitution;
int * NbTermesVecteurDeSubstitution; double * CoeffDeSubstitution;
int * NumeroDeVariableDeSubstitution; int * IndexDansLeTypeDOperationDePresolve;
char * TypeDOperationDePresolve; int * ContrainteDeLaSubstitution;  
int * CorrespondanceCntPneCntEntree;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

*NbModifications = 0;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales quand on fait des substitutions de variables. */
	/* Par contre on peut le faire s'il y a des variables entieres car de toutes façons les variables
	   duales de la solution optimale ne peuvent pas etre interpretees comme des couts marginaux. */
  return;
}

if ( Pne->NbVariablesSubstituees >= Pne->NombreDeVariablesTrav - 1 ) return;
if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;

CoutLineaire = Pne->LTrav;
TypeDeVariableNative = Pne->TypeDeVariableTrav;
NombreDeContraintes = Pne->NombreDeContraintesTrav;
SensContrainte = Pne->SensContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
B = Pne->BTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
CorrespondanceCntPneCntEntree = Pne->CorrespondanceCntPneCntEntree;

ContrainteInactive = Presolve->ContrainteInactive;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
ConserverLaBorneSupDuPresolve = Presolve->ConserverLaBorneSupDuPresolve;
ConserverLaBorneInfDuPresolve = Presolve->ConserverLaBorneInfDuPresolve;

TypeDOperationDePresolve = Pne->TypeDOperationDePresolve;
IndexDansLeTypeDOperationDePresolve = Pne->IndexDansLeTypeDOperationDePresolve;

IndiceDebutVecteurDeSubstitution  = Pne->IndiceDebutVecteurDeSubstitution;
NumeroDesVariablesSubstituees     = Pne->NumeroDesVariablesSubstituees;
CoutDesVariablesSubstituees       = Pne->CoutDesVariablesSubstituees;
ContrainteDeLaSubstitution        = Pne->ContrainteDeLaSubstitution;
ValeurDeLaConstanteDeSubstitution = Pne->ValeurDeLaConstanteDeSubstitution; 
NbTermesVecteurDeSubstitution     = Pne->NbTermesVecteurDeSubstitution; 
CoeffDeSubstitution               = Pne->CoeffDeSubstitution;  
NumeroDeVariableDeSubstitution    = Pne->NumeroDeVariableDeSubstitution;

for ( Cnt = 0 ; Cnt < NombreDeContraintes; Cnt++ ) {

  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;

  if ( SensContrainte[Cnt] != '=' ) continue; 
  /* On recherche les contraintes d'egalite a 2 termes. Etude des intervalles 
     de variation de chacune des 2 variables. Il est necessaire de compter les termes,
     en effet, pour gagner du temps, lorsqu'une variable est substituee, elle n'est pas 
     explicitement supprimee de la contrainte mais son coefficient est mis à 0 ce qui 
     revient au meme. On ne substitue pas une variable entiere par quelque chose
     d'autre */
  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  il1   = -1;  
  il2   = -1;
  SubstitutionPossible = OUI_PNE;
	S = 0.0;
  while ( il < ilMax ) {
    if ( A[il] != 0.0 ) {

		/* Attention ci dessous on passe les variables fixes dans le second membre or les tables
		   de sortie ne sont pas pretes pour un second membre modifie. Il faudra faire la modif */
			 
		  if ( TypeDeBornePourPresolve[Nuvar[il]] == VARIABLE_FIXE ) {
        S += A[il] * ValeurDeXPourPresolve[Nuvar[il]];
			}
			else {
        if ( il1 == -1 ) il1 = il;
        else if ( il2 == -1 ) il2 = il;
        else {
          SubstitutionPossible = NON_PNE;
          break;
        }
			}
    }
    il++;
  }	
  if ( SubstitutionPossible == NON_PNE ) continue;
  if ( il1 < 0 || il2 < 0 ) continue;
  Var1 = Nuvar[il1];
  Var2 = Nuvar[il2];
	
  if ( TypeDeBornePourPresolve[Var1] == VARIABLE_FIXE || TypeDeBornePourPresolve[Var2] == VARIABLE_FIXE ) continue;
  
  CoeffDeVar1 = A[il1];
  CoeffDeVar2 = A[il2];
  
  if ( fabs( CoeffDeVar1 ) < ZERO_PRESOLVE ) continue;
  if ( fabs( CoeffDeVar2 ) < ZERO_PRESOLVE ) continue;

	BCnt = B[Cnt] - S;

	/* On essaie d'exprimer Var1 en fonction de Var2 */
	
  if ( TypeDeVariableNative[Var1] == ENTIER && TypeDeVariableNative[Var2] != ENTIER ) goto TestVar2; /* On ne peut pas remplacer */	
	
	BTilde = BCnt / CoeffDeVar1;
  ATilde = CoeffDeVar2 / CoeffDeVar1;

  /* Si les nombres sont trop differents on ne prend pas a cause des imprecisions */
  if ( fabs( ATilde ) < ATILDE_MIN || fabs( ATilde ) > ATILDE_MAX  ) goto TestVar2; 
	
	if ( TypeDeVariableNative[Var1] == ENTIER && TypeDeVariableNative[Var2] == ENTIER ) {
    if ( ceil( BTilde ) - BTilde > 1.e-10 && BTilde - floor( BTilde ) > 1.e-10 ) goto TestVar2; /* On ne peut pas remplacer */
    if ( ceil( ATilde ) - ATilde > 1.e-10 && ATilde - floor( ATilde ) > 1.e-10 ) goto TestVar2; /* On ne peut pas remplacer */
	}

  if ( PRS_TestSubstituerUneVariable( Presolve, Var1, Var2, ATilde, BTilde, Cnt ) == NON_PNE ) goto TestVar2;

	# if TRACES == 1
    printf("1- Variable %d substituee par la %d et mise en inactivite de la contrainte %d ATilde %lf BTilde %lf\n",Var1,Var2,Cnt,ATilde,BTilde); 
	# endif
 	
  if ( Pne->NbVariablesSubstituees >= Pne->NombreDeVariablesTrav - 1 ) return;
  if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;
		
  TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUBSITUTION_DE_VARIABLE;
  IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbVariablesSubstituees;
	Pne->NombreDOperationsDePresolve++;	
	
  IndiceDebutVecteurDeSubstitution [Pne->NbVariablesSubstituees] = Pne->IndexLibreVecteurDeSubstitution;
  NumeroDesVariablesSubstituees    [Pne->NbVariablesSubstituees] = Var1;
  CoutDesVariablesSubstituees      [Pne->NbVariablesSubstituees] = CoutLineaire[Var1];	
	ContrainteDeLaSubstitution       [Pne->NbVariablesSubstituees] = CorrespondanceCntPneCntEntree[Cnt]; 	
  ValeurDeLaConstanteDeSubstitution[Pne->NbVariablesSubstituees] = BTilde; 
  NbTermesVecteurDeSubstitution    [Pne->NbVariablesSubstituees] = 1;	
  CoeffDeSubstitution           [Pne->IndexLibreVecteurDeSubstitution] = -ATilde;  
  NumeroDeVariableDeSubstitution[Pne->IndexLibreVecteurDeSubstitution] = Var2;	
  Pne->IndexLibreVecteurDeSubstitution++;		
  Pne->NbVariablesSubstituees++;	
	
	ConserverLaBorneSupDuPresolve[Var2] = OUI_PNE;
	ConserverLaBorneInfDuPresolve[Var2] = OUI_PNE;
	
  ContrainteInactive[Cnt] = OUI_PNE;	
  *NbModifications = *NbModifications + 1;

  PRS_SubstituerUneVariable( Presolve, Var1, Var2, ATilde, BTilde );
	
	/* Si necessaire on ajuste les bornes sur Var2 en fonction des bornes sur Var1 */	
  ATilde = -ATilde;
  PRS_CalculerLesNouvellesBornesDeLaVariableRestante( Presolve, Var1, Var2, ATilde, BTilde);

	CoutLineaire[Var1] = 0.0;
  X = 0.; /* Pas d'importance mais ainsi on n'a pas besoin de la supprimer de la liste des 
             variables ni de la supprimer des contraintes */
  PRS_FixerUneVariableAUneValeur( Presolve, Var1, X );
	
  continue;
   
  TestVar2:

	/* On essaie d'exprimer Var2 en fonction de Var1 */
  if ( TypeDeVariableNative[Var2] == ENTIER && TypeDeVariableNative[Var1] != ENTIER ) continue; /* On ne peut pas remplacer */
	
	BTilde = BCnt / CoeffDeVar2;
  ATilde = CoeffDeVar1 / CoeffDeVar2;

  /* Si les nombres sont trop differents on ne prend pas a cause des imprecisions */
  if ( fabs( ATilde ) < ATILDE_MIN || fabs( ATilde ) > ATILDE_MAX  ) continue; 

	if ( TypeDeVariableNative[Var1] == ENTIER && TypeDeVariableNative[Var2] == ENTIER ) {
    if ( ceil( BTilde ) - BTilde > 1.e-10 && BTilde - floor( BTilde ) > 1.e-10 ) continue; /* On ne peut pas remplacer */
    if ( ceil( ATilde ) - ATilde > 1.e-10 && ATilde - floor( ATilde ) > 1.e-10 ) continue; /* On ne peut pas remplacer */
	}
	
  if ( PRS_TestSubstituerUneVariable( Presolve, Var2, Var1, ATilde, BTilde, Cnt ) == NON_PNE ) continue;
			
  # if TRACES == 1   
    printf("2- Variable %d substituee par la %d et mise en inactivite de la contrainte %d ATilde %lf BTilde %lf\n",Var2,Var1,Cnt,ATilde,BTilde);
  # endif
		
  if ( Pne->NbVariablesSubstituees >= Pne->NombreDeVariablesTrav - 1 ) return;
  if ( Pne->NombreDOperationsDePresolve >= Pne->TailleTypeDOperationDePresolve ) return;
	
  TypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = SUBSITUTION_DE_VARIABLE;
  IndexDansLeTypeDOperationDePresolve[Pne->NombreDOperationsDePresolve] = Pne->NbVariablesSubstituees;
	Pne->NombreDOperationsDePresolve++;	
	
  IndiceDebutVecteurDeSubstitution [Pne->NbVariablesSubstituees] = Pne->IndexLibreVecteurDeSubstitution;
  NumeroDesVariablesSubstituees    [Pne->NbVariablesSubstituees] = Var2;
  CoutDesVariablesSubstituees      [Pne->NbVariablesSubstituees] = CoutLineaire[Var2];	
	ContrainteDeLaSubstitution       [Pne->NbVariablesSubstituees] = CorrespondanceCntPneCntEntree[Cnt];	
  ValeurDeLaConstanteDeSubstitution[Pne->NbVariablesSubstituees] = BTilde; 
  NbTermesVecteurDeSubstitution    [Pne->NbVariablesSubstituees] = 1; 
  CoeffDeSubstitution           [Pne->IndexLibreVecteurDeSubstitution] = -ATilde;  
  NumeroDeVariableDeSubstitution[Pne->IndexLibreVecteurDeSubstitution] = Var1;  
  Pne->IndexLibreVecteurDeSubstitution++;
  Pne->NbVariablesSubstituees++;
 		
  ConserverLaBorneSupDuPresolve[Var1] = OUI_PNE;
	ConserverLaBorneInfDuPresolve[Var1] = OUI_PNE;
			
  ContrainteInactive[Cnt] = OUI_PNE;	
  *NbModifications = *NbModifications + 1;
	
  PRS_SubstituerUneVariable( Presolve, Var2, Var1, ATilde, BTilde );
	
	/* Si necessaire on ajuste les bornes sur Var1 en fonction des bornes sur Var2 */	
  ATilde = -ATilde;
  PRS_CalculerLesNouvellesBornesDeLaVariableRestante( Presolve, Var2, Var1, ATilde, BTilde);

	CoutLineaire[Var2] = 0.0;
  X = 0.; /* Pas d'importance mais ainsi on n'a pas besoin de la supprimer de la liste des 
             variables ni de la supprimer des contraintes */
  PRS_FixerUneVariableAUneValeur( Presolve, Var2, X );   
	
  continue;
  
}

#if VERBOSE_PRS
  printf("-> Nombre de contraintes supprimees par substitution de variables %d\n",*NbModifications); 
#endif

if ( *NbModifications > 0 && 0 ) { 
  PRS_ClasserVariablesDansContraintes( Presolve ); /* Ca n'a plus d'utilite */
  PNE_ConstruireLeChainageDeLaTransposee( Pne );
}

return;
}

/*----------------------------------------------------------------------------*/

void PRS_CalculerLesNouvellesBornesDeLaVariableRestante( PRESOLVE * Presolve,
                                                         int VarSubstituee,
                                                         int VarRestante, 
                                                         double ATilde,
                                                         double BTilde )
{
int TypeBorne; char XmaxExiste; char XminExiste; double Xmax; double Xmin;
double X; PROBLEME_PNE * Pne;

Pne = Presolve->ProblemePneDuPresolve;

TypeBorne = Presolve->TypeDeBornePourPresolve[VarSubstituee];
XmaxExiste = NON_PNE;
XminExiste = NON_PNE;
Xmin = 0;
Xmax = 0;
if ( ATilde > 0.0 ) {
  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    Xmax = ( Presolve->BorneSupPourPresolve[VarSubstituee] - BTilde ) / ATilde;
		XmaxExiste = OUI_PNE;
	}
	if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
    Xmin = ( Presolve->BorneInfPourPresolve[VarSubstituee] - BTilde ) / ATilde;
    XminExiste = OUI_PNE;
	}
}
else {
  if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
    Xmax = ( Presolve->BorneInfPourPresolve[VarSubstituee] - BTilde ) / ATilde;
		XmaxExiste = OUI_PNE;
	}
	if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    Xmin = ( Presolve->BorneSupPourPresolve[VarSubstituee] - BTilde ) / ATilde;
    XminExiste = OUI_PNE;
	}
}

TypeBorne = Presolve->TypeDeBornePourPresolve[VarRestante];
if ( XminExiste == OUI_PNE ) {
	if ( TypeBorne == VARIABLE_NON_BORNEE || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    Presolve->BorneInfPourPresolve[VarRestante] = Xmin;
		if ( TypeBorne == VARIABLE_NON_BORNEE ) TypeBorne = VARIABLE_BORNEE_INFERIEUREMENT;
		else if ( TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) TypeBorne = VARIABLE_BORNEE_DES_DEUX_COTES;
	}
	else if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
    if ( Xmin > Presolve->BorneInfPourPresolve[VarRestante] ) {
		  Presolve->BorneInfPourPresolve[VarRestante] = Xmin;
		}
	}    
}
if ( XmaxExiste == OUI_PNE ) {
	if ( TypeBorne == VARIABLE_NON_BORNEE || TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) {
    Presolve->BorneSupPourPresolve[VarRestante] = Xmax;
		if ( TypeBorne == VARIABLE_NON_BORNEE ) TypeBorne = VARIABLE_BORNEE_SUPERIEUREMENT;
		else if ( TypeBorne == VARIABLE_BORNEE_INFERIEUREMENT ) TypeBorne = VARIABLE_BORNEE_DES_DEUX_COTES;
	}
	else if ( TypeBorne == VARIABLE_BORNEE_DES_DEUX_COTES || TypeBorne == VARIABLE_BORNEE_SUPERIEUREMENT ) {
    if ( Xmax < Presolve->BorneSupPourPresolve[VarRestante] ) {
		  Presolve->BorneSupPourPresolve[VarRestante] = Xmax;
		}
	} 
}

Presolve->TypeDeBornePourPresolve[VarRestante] = TypeBorne;
if ( Pne->TypeDeVariableTrav[VarRestante] == ENTIER ) {	
  if ( Presolve->BorneSupPourPresolve[VarRestante] < Pne->UmaxTrav[VarRestante] - 1.e-7 && 
       Presolve->BorneInfPourPresolve[VarRestante] > Pne->UminTrav[VarRestante] + 1.e-7 ) {
	  Pne->YaUneSolution = PROBLEME_INFAISABLE;
		return;
	}
  if ( Presolve->BorneSupPourPresolve[VarRestante] < Pne->UmaxTrav[VarRestante] - 1.e-7 ) {
	  X = Pne->UminTrav[VarRestante];
    PRS_FixerUneVariableAUneValeur( Presolve, VarRestante, X );   
	}
	else if ( Presolve->BorneInfPourPresolve[VarRestante] > Pne->UminTrav[VarRestante] + 1.e-7 ) {
	  X = Pne->UmaxTrav[VarRestante];
    PRS_FixerUneVariableAUneValeur( Presolve, VarRestante, X );   
	}
}

/* 14/01/2015: Il faut reajuster ConserverLaBorneSupDuPresolve et ConserverLaBorneInfDuPresolve */
if ( Presolve->TypeDeBornePourPresolve[VarRestante] == VARIABLE_BORNEE_INFERIEUREMENT ) {
	Presolve->ConserverLaBorneSupDuPresolve[VarRestante] = NON_PNE;
}
else if ( Presolve->TypeDeBornePourPresolve[VarRestante] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
	Presolve->ConserverLaBorneInfDuPresolve[VarRestante] = NON_PNE;
}
else if ( Presolve->TypeDeBornePourPresolve[VarRestante] == VARIABLE_NON_BORNEE ) {
	Presolve->ConserverLaBorneSupDuPresolve[VarRestante] = NON_PNE;
	Presolve->ConserverLaBorneInfDuPresolve[VarRestante] = NON_PNE;
}

return;
}

/*----------------------------------------------------------------------------*/

char PRS_TestSubstituerUneVariable( PRESOLVE * Presolve,
                                    int   VarSubstituee,
                                    int   VarRestante, 
                                    double ATilde,
                                    double BTilde,
			                              int   CntSubstitution )
{
int il ; double Ai; int Cnt; int ilCnt; int ilMaxCnt; char VarRestanteTrouvee; 
double X ; PROBLEME_PNE * Pne; int * Cdeb; int * NumContrainte; int * Mdeb;
int * NbTerm; int * Nuvar; int * Csui; double * A; char * ContrainteInactive;
double PlusGrandTerme; double PlusPetitTerme;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
 
# if TRACES != 1
  CntSubstitution = -1; /* Pour ne pas avoir de warning a la compilation */
# endif

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
A = Pne->ATrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
PlusGrandTerme = Pne->PlusGrandTerme;
PlusPetitTerme = Pne->PlusPetitTerme;

ContrainteInactive = Presolve->ContrainteInactive;

BTilde = 0.; /* Pour ne pas avoir de warning a la compilation */

/* On balaye la colonne de la variable substituee */
il  = Cdeb[VarSubstituee];
while ( il >= 0 ) {   
  Cnt = NumContrainte[il];	
  if ( ContrainteInactive[Cnt] == OUI_PNE ) goto Next_il;
  VarRestanteTrouvee = NON_PNE;  
  Ai = A[il];
  /* On balaye la contrainte pour faire les actions necessaires au cas ou 
     la variable VarRestante y serait deja presente */
  ilCnt    = Mdeb[Cnt];
  ilMaxCnt = ilCnt + NbTerm[Cnt];
  while ( ilCnt < ilMaxCnt ) {
    if ( Nuvar[ilCnt] == VarRestante ) {		
      X = fabs( A[ilCnt] - (Ai * ATilde) );	
      if ( X > PlusGrandTerme || (X < PlusPetitTerme && X != 0.0 ) ) {
        /* On refuse la substitution de variable */
	      # if TRACES == 1
	        printf("refus 1 car Cnt %d CntSubstitution %d X = %e ATrav = %e Ai = %e ATilde = %e \n",Cnt,CntSubstitution,X,Pne->ATrav[ilCnt],Ai,ATilde);
	      # endif
	      return( NON_PNE );
      }			
      VarRestanteTrouvee = OUI_PNE;
      break;  
    } 
    ilCnt++;
  }
  if ( VarRestanteTrouvee == NON_PNE ) {
    X = fabs( -Ai * ATilde );		
    if ( X > PlusGrandTerme || (X < PlusPetitTerme && X != 0.0 ) ) {
      /* On refuse la substitution de variable */
      # if TRACES == 1
        printf("refus 2 car Cnt %d CntSubstitution %d X = %e Ai = %e ATilde = %e \n",Cnt,CntSubstitution,X,Ai,ATilde);
      # endif
      return( NON_PNE );
    }                
  }  
  Next_il:
  il = Csui[il];    
}

return( OUI_PNE );
}  

/*----------------------------------------------------------------------------*/

void PRS_SubstituerUneVariable( PRESOLVE * Presolve,
                                int   VarSubstituee,
                                int   VarRestante, 
                                double ATilde,
                                double BTilde )
{
int il ; long double Ai    ; int Cnt; int ilCnt; int ilMaxCnt; char VarRestanteTrouvee; 
int ilR; int   ilPrec; PROBLEME_PNE * Pne; int * Cdeb; int * NumContrainte;
int * Mdeb; int * NbTerm; int * Nuvar; int * Csui; double * A; double * B;
char * ContrainteInactive; double * CoutLineaire; 

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

/* On balaye la colonne de la variable substituee */

Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
CoutLineaire = Pne->LTrav;
NumContrainte = Pne->NumContrainteTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
B = Pne->BTrav;
ContrainteInactive = Presolve->ContrainteInactive;

il  = Cdeb[VarSubstituee];
while ( il >= 0 ) {
  Cnt = NumContrainte[il];	
  VarRestanteTrouvee = NON_PNE;	
  /* Pas de substitution sur des contraintes inactives */	
  Ai = (long double) A[il];	
  if ( ContrainteInactive[Cnt] != OUI_PNE && Ai != 0.0 ) {	
    /* On balaye la contrainte pour faire les actions necessaires au cas ou 
       la variable VarRestante y serait deja presente */
    ilCnt    = Mdeb[Cnt];
    ilMaxCnt = ilCnt + NbTerm[Cnt];
    while ( ilCnt < ilMaxCnt ) {
      if ( Nuvar[ilCnt] == VarRestante ) {							
        A[ilCnt] -= Ai * (long double) ATilde;				
        VarRestanteTrouvee = OUI_PNE;  
        break;
      } 
      ilCnt++;
    }
    B[Cnt] -= Ai * (long double) BTilde;
  }

  ilPrec = il;
  il = Csui[il];

  if ( VarRestanteTrouvee == NON_PNE && ContrainteInactive[Cnt] != OUI_PNE && Ai != 0.0 ) { 
    Nuvar[ilPrec] = VarRestante;		
    A[ilPrec] = -Ai * (long double) ATilde;		
    /* Attention comme on a change la colonne d'un terme il faut modifier le chainage 
       de la colonne VarRestante */
    ilR = Cdeb[VarRestante];
    Cdeb[VarRestante] = ilPrec; 
    Csui[ilPrec] = ilR;         
  }
  else {
    /* On peut mettre le coefficient de la variable substituee à 0 s'il y avait la 
       variable restante dans la contrainte */
    A[ilPrec] = 0.;  
  }	
}

CoutLineaire[VarRestante] -= CoutLineaire[VarSubstituee] * ATilde;

Pne->Z0 += CoutLineaire[VarSubstituee] * BTilde;

Cdeb[VarSubstituee] = -1;

return;
}  

/*----------------------------------------------------------------------------*/

void PRS_ClasserVariablesDansContraintes( PRESOLVE * Presolve )
{

int Cnt; char OnContinue; int NombreDeContraintesTrav; int ilDeb; int il;
int ilMax; int Var1; double A1; int * MdebTrav; int * NbTermTrav;
int * NuvarTrav; double * ATrav; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeContraintesTrav = Pne->NombreDeContraintesTrav;

MdebTrav   = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav  = Pne->NuvarTrav;
ATrav      = Pne->ATrav;

for ( Cnt = 0 ; Cnt < NombreDeContraintesTrav ; Cnt++ ) {
  ilDeb = MdebTrav[Cnt];
  ilMax = ilDeb + NbTermTrav[Cnt];
  ilMax--;
  OnContinue = OUI_PNE;	
  while ( OnContinue == OUI_PNE ) {
    OnContinue = NON_PNE;
    il = ilDeb;
    while ( il < ilMax ) {
      if ( NuvarTrav[il] > NuvarTrav[il+1] ) {
        OnContinue = OUI_PNE;
        Var1 = NuvarTrav[il];
        A1   = ATrav[il];
        NuvarTrav[il] = NuvarTrav[il+1];
        ATrav    [il] = ATrav[il+1];
        NuvarTrav[il+1] = Var1;
        ATrav    [il+1] = A1;      
      }
      il++;
    }
  }
}

return;
}


