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

   FONCTION: Amelioration des bornes
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"

# include "pne_define.h"

# include "prs_define.h"  

# define EPS_UMAX_UMIN 1.e-5

# define TRACES 0
# define MAX_ITER 10
# define COEFF_MIN 1.e-15

/*----------------------------------------------------------------------------*/
/*              On regarde si on peut fixer des variables entieres ou
                resserer des bornes sur les variables                         */                     

void PRS_AmeliorerLesBornes( PRESOLVE * Presolve, int * BorneAmelioree )
{
int Var; int NbBornesAmeliorees; int Nb; int NbIter; int * TypeDeBornePourPresolve;
int NombreDeVariables; int NbModifs; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NbBornesAmeliorees = 0; 
NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

*BorneAmelioree = NON_PNE;

NbIter = 0;
while ( NbIter < MAX_ITER ) {
  NbIter++;
  Nb = NbBornesAmeliorees;

  for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {		
    if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;
    PRS_AmeliorerBorneSurVariable( Presolve, Var, &NbBornesAmeliorees );
	  if ( Pne->YaUneSolution == PROBLEME_INFAISABLE ) {
		  *BorneAmelioree = NON_PNE;
		  return;
	  }		 	
  }

	if ( NbBornesAmeliorees == Nb ) break;
	
  PRS_CalculerLesBornesDeToutesLesContraintes( Presolve , &NbModifs ); 
	if ( Pne->YaUneSolution != OUI_PNE ) break;
		
}
   
#if VERBOSE_PRS
  printf("-> Nombre de bornes amelioree: %d\n",NbBornesAmeliorees);
#endif

if ( NbBornesAmeliorees != 0 ) *BorneAmelioree = OUI_PNE;

return;
}
  
/*----------------------------------------------------------------------------*/

void PRS_AmeliorerBorneSurVariable( PRESOLVE * Presolve, int Var, int * Nbs )
{
int il; int Cnt; double CoeffDeVar; double Smin; double Smax; double AxiMin;
double AxiMax; double XiMin; double XiMax; double InfDesXiMax;
double SupDesXiMin; char InfDesXiMaxEstInitialise; double * A; int TypeBrn;
char SupDesXiMinEstInitialise; char SminEstValide; char SmaxEstValide; char XiMinEstValide;
char XiMaxEstValide; int ContrainteBornanteSup; int ContrainteBornanteInf;
char * MinContrainteCalcule; double * BorneSupPourPresolve;
char * MaxContrainteCalcule; char * ContrainteInactive; int * Cdeb; int * Csui;
int * NumContrainte; char * SensContrainte; double * B; double * MinContrainte; double * MaxContrainte;
int * TypeDeBornePourPresolve; 
double Xmx; double Xmn; int * TypeDeVariable; double * BorneInfPourPresolve; PROBLEME_PNE * Pne;
double * ValeurDeXPourPresolve; char BorneAmelioree;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
MinContrainteCalcule = Presolve->MinContrainteCalcule;
MaxContrainteCalcule = Presolve->MaxContrainteCalcule;
MinContrainte = Presolve->MinContrainte;
MaxContrainte = Presolve->MaxContrainte;
ContrainteInactive = Presolve->ContrainteInactive;

A = Pne->ATrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
NumContrainte = Pne->NumContrainteTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;

InfDesXiMaxEstInitialise = NON_PNE;
SupDesXiMinEstInitialise = NON_PNE;
ContrainteBornanteSup = -1;  
ContrainteBornanteInf = -1;

/* Pour eviter les warning de compilation */
Smin = 0.;
Smax = 0.;
XiMin = 0.; 
SupDesXiMin = 0.; 
XiMax = 0.; 
InfDesXiMax = 0.; 
CoeffDeVar = 1.;

TypeBrn = TypeDeBornePourPresolve[Var];
Xmx = BorneSupPourPresolve[Var];
Xmn = BorneInfPourPresolve[Var];

il = Cdeb[Var];
while ( il >= 0 ) {

  Cnt = NumContrainte[il];
  if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante;

  CoeffDeVar = A[il];
	if ( fabs( CoeffDeVar ) < PIVOT_MIN_POUR_UN_CALCUL_DE_BORNE ) goto ContrainteSuivante;
		
  SmaxEstValide  = OUI_PNE;
  SminEstValide  = OUI_PNE;
  XiMinEstValide = OUI_PNE;  
  XiMaxEstValide = OUI_PNE;
	
  if ( SensContrainte[Cnt] == '=' ) { 
    /* Dans le cas d'une contrainte d'egalite on peut calculer une borne 
       min et une borne max */
    if ( CoeffDeVar > ZERO_PRESOLVE ) { 
      /* Coeff positif */
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT || MaxContrainteCalcule[Cnt] == NON_PNE ) {
			  SmaxEstValide = NON_PNE;
			}
      else {			
			  if ( fabs( CoeffDeVar * Xmx ) < COEFF_MIN * fabs( MaxContrainte[Cnt] ) ) SmaxEstValide = NON_PNE;				
			  Smax = MaxContrainte[Cnt] - ( CoeffDeVar * Xmx );
			}
			
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT || MinContrainteCalcule[Cnt] == NON_PNE ) {
			  SminEstValide = NON_PNE;
			}
      else {
			  if ( fabs( CoeffDeVar * Xmn ) < COEFF_MIN * fabs( MinContrainte[Cnt] ) ) SminEstValide = NON_PNE;
			  Smin = MinContrainte[Cnt] - ( CoeffDeVar * Xmn );
      }
			
      AxiMin = B[Cnt] - Smax;
      AxiMax = B[Cnt] - Smin;

      if ( SmaxEstValide == NON_PNE ) XiMinEstValide = NON_PNE;
      else XiMin = AxiMin / CoeffDeVar; 
      if ( SminEstValide == NON_PNE ) XiMaxEstValide = NON_PNE;
      else XiMax = AxiMax / CoeffDeVar; 
    }
    else if ( CoeffDeVar < -ZERO_PRESOLVE ) {
      /* Coeff negatif */
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT || MaxContrainteCalcule[Cnt] == NON_PNE ) {
			  SmaxEstValide = NON_PNE;
			}
      else {
			  if ( fabs( CoeffDeVar * Xmn ) < COEFF_MIN * fabs( MaxContrainte[Cnt] ) ) SmaxEstValide = NON_PNE;
			  Smax = MaxContrainte[Cnt] - ( CoeffDeVar * Xmn );
			}
			
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT || MinContrainteCalcule[Cnt] == NON_PNE ) {
			  SminEstValide = NON_PNE;
			}
      else {
			  if ( fabs( CoeffDeVar * Xmx ) < COEFF_MIN * fabs( MinContrainte[Cnt] ) ) SminEstValide = NON_PNE;
			  Smin = MinContrainte[Cnt] - ( CoeffDeVar * Xmx );
      }
			
      AxiMin = B[Cnt] - Smax;
      AxiMax = B[Cnt] - Smin;

      if ( SminEstValide == NON_PNE ) XiMinEstValide = NON_PNE;
      else XiMin  = AxiMax / CoeffDeVar; 
      if ( SmaxEstValide == NON_PNE ) XiMaxEstValide = NON_PNE;
      else XiMax  = AxiMin / CoeffDeVar; 
    }
    else return; /* Car evaluation impossible */
    if ( XiMinEstValide == OUI_PNE ) {
      if ( SupDesXiMinEstInitialise == OUI_PNE ) {
        if ( XiMin > SupDesXiMin ) { SupDesXiMin = XiMin; ContrainteBornanteInf = Cnt; }
      }
      else {
        SupDesXiMin = XiMin;
        SupDesXiMinEstInitialise = OUI_PNE;
	      ContrainteBornanteInf = Cnt;
      }
    }
    if ( XiMaxEstValide == OUI_PNE ) {
      if ( InfDesXiMaxEstInitialise == OUI_PNE ) {
        if ( XiMax < InfDesXiMax ) { InfDesXiMax = XiMax; ContrainteBornanteSup = Cnt; }
      }
      else {
        InfDesXiMax = XiMax;
        InfDesXiMaxEstInitialise = OUI_PNE;
	      ContrainteBornanteSup = Cnt;
      }
    }
  }
  else {     
    /* Dans le cas d'une contrainte d'inegalite on ne peut calculer qu'un seul type de borne */
    if ( CoeffDeVar > ZERO_PRESOLVE ) { 
      /* Coeff positif */
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_SUPERIEUREMENT || MinContrainteCalcule[Cnt] == NON_PNE ) {
			  SminEstValide = NON_PNE;
			}			
      else {
			  if ( fabs( CoeffDeVar * Xmn ) < COEFF_MIN * fabs( MinContrainte[Cnt] ) ) SminEstValide = NON_PNE;
      }			
      if ( SminEstValide == OUI_PNE ) {
        Smin = MinContrainte[Cnt] - ( CoeffDeVar * Xmn );
        AxiMax = B[Cnt] - Smin;
        XiMax  = AxiMax / CoeffDeVar; 
        if ( InfDesXiMaxEstInitialise == OUI_PNE ) {
          if ( XiMax < InfDesXiMax ) { InfDesXiMax = XiMax; ContrainteBornanteSup = Cnt; }
        }
        else {
          InfDesXiMax = XiMax;
          InfDesXiMaxEstInitialise = OUI_PNE;
	        ContrainteBornanteSup = Cnt;
        }
      }
    }
    else if ( CoeffDeVar < -ZERO_PRESOLVE ) {  
      /* Coeff negatif */
      if ( TypeBrn == VARIABLE_NON_BORNEE || TypeBrn == VARIABLE_BORNEE_INFERIEUREMENT || MinContrainteCalcule[Cnt] == NON_PNE ) {
			  SminEstValide = NON_PNE;
			}
      else {
			  if ( fabs( CoeffDeVar * Xmx ) < COEFF_MIN * fabs( MinContrainte[Cnt] ) ) SminEstValide = NON_PNE;
      }				
      if ( SminEstValide == OUI_PNE ) {
        Smin = MinContrainte[Cnt] - ( CoeffDeVar * Xmx );
        AxiMax = B[Cnt] - Smin;
        XiMin  = AxiMax / CoeffDeVar; 
        if ( SupDesXiMinEstInitialise == OUI_PNE ) {
          if ( XiMin > SupDesXiMin ) { SupDesXiMin = XiMin; ContrainteBornanteInf = Cnt; }
        }
        else {
          SupDesXiMin = XiMin;
          SupDesXiMinEstInitialise = OUI_PNE;
	        ContrainteBornanteInf = Cnt;
        }
      }
    }
    else return; /* Car evaluation impossible */
  }
  ContrainteSuivante:
  il = Csui[il];

}

if ( InfDesXiMaxEstInitialise == OUI_PNE ) {
  if ( fabs( InfDesXiMax ) > VALEUR_DE_BORNE_DEBILE ) {
   /* Nouvelle borne refusee */
   InfDesXiMaxEstInitialise = NON_PNE;
  }
}
if ( SupDesXiMinEstInitialise == OUI_PNE ) {
  if ( fabs( SupDesXiMin ) > VALEUR_DE_BORNE_DEBILE ) {
    /* Nouvelle borne refusee */
    SupDesXiMinEstInitialise = NON_PNE;
  }
}

/* Analyse des bornes eventuellement calculees pour la variable */
if ( InfDesXiMaxEstInitialise == NON_PNE && SupDesXiMinEstInitialise == NON_PNE ) return;

/* Si InfDesXiMaxEstInitialise = OUI_PNE on a une borne Sup */
/* Si SupDesXiMinEstInitialise = OUI_PNE on a une borne Inf */

PRS_MettreAJourLesBornesDUneVariable( Presolve, Var, SupDesXiMinEstInitialise, SupDesXiMin,
																		  ContrainteBornanteInf, InfDesXiMaxEstInitialise, InfDesXiMax,
																	 	  ContrainteBornanteSup, &BorneAmelioree );

if ( BorneAmelioree == OUI_PNE ) *Nbs = *Nbs + 1;

return;

}




