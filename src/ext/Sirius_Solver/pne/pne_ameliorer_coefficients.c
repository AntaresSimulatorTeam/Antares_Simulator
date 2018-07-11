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
						 Bien que ce calcul soit fait dans le presolve, on le fait
						 aussi apres le variable probing et la prise en compte
						 des coupes de probing car l'ensemble des contraintes n'est
						 plus le meme.
						 
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  
  
# include "pne_fonctions.h"
# include "pne_define.h"

# include "prs_define.h"
# include "prs_fonctions.h"
  
# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

# define TRACES 0

/*----------------------------------------------------------------------------*/

void PNE_AmeliorerLesCoefficientsDesVariablesBinaires( PROBLEME_PNE * Pne,
                                                       void * Prs,
																											 char Mode ) 

{
int Var; int Cnt; int il; double Smax; int ilEntier; int NombreDeVariables; double Coeff;
int * TypeDeVariable; int * Cdeb; int * Csui; int * NumContrainte; char * SensContrainte;
int * Mdeb; int * NbTerm; int * Nuvar; double a; double * B; double * A; double PlusPetitTerme;
double * Xmin; double * Xmax; int NbIter; char CoeffModifie; double * Bmax; char * BmaxValide;
char * BorneInfConnue; int * TypeDeBornePourPresolve; char * ContrainteInactive;
int Faisabilite; double Marge; int NbModifications; int NbC; PRESOLVE * Presolve;

if ( Pne->YaDesVariablesEntieres != OUI_PNE ) return;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

Presolve = (PRESOLVE *) Prs;

if ( Mode == MODE_PNE ) {
  /*
  printf( "On n'utilise pas AmeliorerLesCoefficientsDesVariablesBinaires si MODE_PNE \n");
  return;
	*/
  # if TRACES == 1
	  printf("AmeliorerLesCoefficientsDesVariablesBinaires: MODE_PNE\n");
	# endif
	
  PNE_InitBorneInfBorneSupDesVariables( Pne );	
  PNE_CalculMinEtMaxDesContraintes( Pne, &Faisabilite );	
}
else if ( Mode == MODE_PRESOLVE ) {
  # if TRACES == 1
	  printf("AmeliorerLesCoefficientsDesVariablesBinaires: MODE_PRESOLVE\n");
	# endif	
  PRS_CalculerLesBornesDeToutesLesContraintes( Presolve, &NbModifications );	
}
else return;

Marge = MARGE_REDUCTION;

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
PlusPetitTerme = Pne->PlusPetitTerme;
NbC = 0;

Xmin = NULL;
Xmax = NULL;
BorneInfConnue = NULL;
BmaxValide = NULL;
Bmax = NULL;
TypeDeBornePourPresolve = NULL;
ContrainteInactive = NULL;

if ( Mode == MODE_PNE ) {
  Xmin = Pne->ProbingOuNodePresolve->ValeurDeBorneInf;
  Xmax = Pne->ProbingOuNodePresolve->ValeurDeBorneSup;
  BorneInfConnue = Pne->ProbingOuNodePresolve->BorneInfConnue;
  BmaxValide = Pne->ProbingOuNodePresolve->BmaxValide;
  Bmax = Pne->ProbingOuNodePresolve->Bmax;
}
else {
  Xmin = Presolve->BorneInfPourPresolve;
  Xmax = Presolve->BorneSupPourPresolve;
  TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
  ContrainteInactive = Presolve->ContrainteInactive;
  BmaxValide = Presolve->MaxContrainteCalcule;
  Bmax = Presolve->MaxContrainte;
}

NbIter = 0;
Debut:
CoeffModifie = NON_PNE;
for ( Var = 0 ; Var < NombreDeVariables ; Var++ ) {
  if ( TypeDeVariable[Var] != ENTIER ) continue;
	if ( Mode == MODE_PNE ) {
    if ( BorneInfConnue[Var] == FIXE_AU_DEPART ) continue;
	}
	else {
    if ( TypeDeBornePourPresolve[Var] == VARIABLE_FIXE ) continue;
	}  
  il = Cdeb[Var];
  while ( il >= 0 ) {
    Cnt = NumContrainte[il];
    if ( SensContrainte[Cnt] == '=' ) goto ContrainteSuivante;    		   
	  if ( Mode == MODE_PRESOLVE ) {
      if ( ContrainteInactive[Cnt] == OUI_PNE ) goto ContrainteSuivante;
		}		
    /* La contrainte est donc de type < , calcul du max du membre de gauche */
		ilEntier = il;
		Coeff = A[ilEntier];
		if ( BmaxValide[Cnt] == OUI_PNE ) Smax = Bmax[Cnt];		
		else goto ContrainteSuivante;
		
		if ( Smax <= B[Cnt] ) goto ContrainteSuivante; /* Contrainte redondante */    
		
    if ( Coeff < 0.0 ) {					
      if ( Smax + Coeff < B[Cnt] - EPS_COEFF_REDUCTION ) {  
        /* On peut diminuer le coeff de la variable entiere */
				a = B[Cnt] - Smax - Marge;
        if ( fabs( a ) < PlusPetitTerme ) {
					a = -PlusPetitTerme;
				}
				if ( a < Coeff ) goto ContrainteSuivante;
				if ( fabs( a ) < fabs( Coeff) - DELTA_MIN_REDUCTION ) {				
					# if TRACES == 1
	          printf("  Variable entiere %d contrainte %d, remplacement de son coefficient %e par %e \n",Var,Cnt,Coeff,a);
			    # endif					
					/* Mise a jour de MaxContrainte[Cnt] */
          Bmax[Cnt] -= Coeff * Xmin[Var];
          Bmax[Cnt] += a * Xmin[Var]; /* Meme si on sait que borne inf = 0 */
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
				  a = PlusPetitTerme;
				}				
				if ( a > Coeff ) goto ContrainteSuivante;
				if ( fabs( a ) < fabs( Coeff) - DELTA_MIN_REDUCTION ) {				  									
					# if TRACES == 1					
				    printf("  Variable entiere %d contrainte %d, remplacement de son coefficient %e par %e et B %e par %e\n",Var,Cnt,Coeff,a,
				              B[Cnt],Smax-Coeff+Marge);					
			    # endif
					/* Mise a jour de MaxContrainte[Cnt] */
          Bmax[Cnt] -= Coeff * Xmax[Var];
          Bmax[Cnt] += a * Xmax[Var];
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

