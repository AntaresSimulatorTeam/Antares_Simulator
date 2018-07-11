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

   FONCTION: Amelioration des coefficients des variables entieres 
             (cas des variables binaires uniquement).
						 La description de la methode se trouve dans l'article:
						 "Computational Integer Programming and cutting planes"
             Armin Fugenschuh & Alexander Martin, page 6, 2001.
						 Mais on peut aussi la trouver dans d'autres articles.
						 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PRS_AmeliorerLesCoefficientsDesVariablesBinaires( PRESOLVE * Presolve ) 

{
int Var; int Cnt; int il; double Smax; int ilEntier; int NombreDeVariables; double Coeff;
char * ContrainteInactive; int * TypeDeVariable; int * Cdeb; int * Csui; int * NumContrainte;
char * SensContrainte; int * Mdeb; int * NbTerm; int * Nuvar; int * TypeDeBornePourPresolve;
double a; double * B; double * A; double * BorneInfPourPresolve; double * BorneSupPourPresolve;
int NbIter; char CoeffModifie; PROBLEME_PNE * Pne; int NbModifications; int NbC;
double * MaxContrainte; char * MaxContrainteCalcule; double Marge; double PlusPetitTerme;
   
Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

if ( Pne->YaDesVariablesEntieres != OUI_PNE ) return;

Marge = MARGE_REDUCTION;
PlusPetitTerme = Pne->PlusPetitTerme;
NbC = 0;

PRS_CalculerLesBornesDeToutesLesContraintes( Presolve, &NbModifications );	

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;


NombreDeVariables = Pne->NombreDeVariablesTrav;
TypeDeVariable = Pne->TypeDeVariableTrav;
Cdeb = Pne->CdebTrav;
Csui = Pne->CsuiTrav;
NumContrainte = Pne->NumContrainteTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;

ContrainteInactive = Presolve->ContrainteInactive;
MaxContrainteCalcule = Presolve->MaxContrainteCalcule;
MaxContrainte = Presolve->MaxContrainte;

NbIter = 0;
Debut:
CoeffModifie = NON_PNE;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeVariable[Var] != ENTIER ) continue;
  if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;

  il = Cdeb[Var];
  while ( il >= 0 ) {
    Cnt = NumContrainte[il];
    if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante;
    if ( SensContrainte[Cnt] == '=' ) goto ContrainteSuivante;    		   
    /* La contrainte est donc de type < , calcul du max du membre de gauche */
		ilEntier = il;
		Coeff = A[ilEntier];
		if ( MaxContrainteCalcule[Cnt] == OUI_PNE ) Smax = MaxContrainte[Cnt];		
		else goto ContrainteSuivante;
		
		if ( Smax <= B[Cnt] ) goto ContrainteSuivante; /* Contrainte redondante */    
		
    if ( Coeff < 0.0 ) {		  
      if ( Smax + Coeff < B[Cnt] - EPS_COEFF_REDUCTION ) {  
        /* On peut diminuer le coeff de la variable entiere */
				a = B[Cnt] - Smax - Marge;
        if ( fabs( a ) < PlusPetitTerme ) {
				  goto ContrainteSuivante;
				  a = -PlusPetitTerme;
				}
				if ( a < Coeff ) goto ContrainteSuivante;				
				if ( fabs( a ) < fabs( Coeff) - DELTA_MIN_REDUCTION ) {				  
					# if TRACES == 1
	          printf("  Variable entiere %d contrainte %d, remplacement de son coefficient %e par %e \n",Var,Cnt,Coeff,a);
			    # endif
					/* Mise a jour de MaxContrainte[Cnt] */
          MaxContrainte[Cnt] -= Coeff * BorneInfPourPresolve[Var];
          MaxContrainte[Cnt] += a * BorneInfPourPresolve[Var]; /* Meme si on sait que borne inf = 0 */
					/* Modif du coefficient */
          A[ilEntier] = a;
					CoeffModifie = OUI_PNE;
					NbC++;
				}
			}
		}
		else if ( Coeff > 0.0 ) {
      if ( Smax - Coeff < B[Cnt] - EPS_COEFF_REDUCTION ) {
        /* On peut diminuer le coeff de la variable entiere */
				a = Smax - B[Cnt] + Marge;
        if ( fabs( a ) < PlusPetitTerme ) {
				  goto ContrainteSuivante;
				  a = PlusPetitTerme;
				}	
				if ( a > Coeff ) goto ContrainteSuivante;				
				if ( fabs( a ) < fabs( Coeff) - DELTA_MIN_REDUCTION ) {          
					# if TRACES == 1					
				    printf("  Variable entiere %d contrainte %d, remplacement de son coefficient %e par %e et B %e par %e\n",Var,Cnt,Coeff,a,
				              B[Cnt],Smax-Coeff+Marge);					
			    # endif
					/* Mise a jour de MaxContrainte[Cnt] */
          MaxContrainte[Cnt] -= Coeff * BorneSupPourPresolve[Var];
          MaxContrainte[Cnt] += a * BorneSupPourPresolve[Var];
					/* Modif du coefficient */					
				  A[ilEntier] = a;
				  B[Cnt] = Smax - Coeff + Marge;
					CoeffModifie = OUI_PNE;				
					NbC++;
				}
		  }
		}
    ContrainteSuivante:
    il = Csui[il];
  }
}
if ( CoeffModifie == OUI_PNE ) {
	# if TRACES == 1					
    printf("AmeliorerLesCoefficientsDesVariablesBinaires : NbIter %d\n",NbIter);
	# endif
  NbIter++;
  if ( NbIter < NB_ITER_MX_REDUCTION ) goto Debut;
}

if ( Pne->AffichageDesTraces == OUI_PNE ) {
  if ( NbC != 0 ) printf("%d binary coefficient(s) reduced\n",NbC);
}

return;
}

