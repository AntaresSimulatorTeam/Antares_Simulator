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

   FONCTION: Il s'agit de la reprise d'une partie du presolve.
	           On cherche a fixer les variables entieres dont le cout est nul.
						 - Si toutes les inegalites ou elle a un coeff negatif sont telles
						   que la plus grande borne inf induite par ces inegalites est inferieure
							 ou egale a sa borne native alors on fixe la variable a sa borne
							 inf native.
						 - Si toutes les inegalites ou elle a un coeff positif sont telles
						   que la plus petite borne sup induite par ces inegalites est superieure
							 ou egale a sa borne native alors on fixe la variable a sa borne
							 sup native.							 		
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"  

# include "pne_fonctions.h"
# include "pne_define.h"

# include "spx_define.h"

# include "bb_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PNE_FixationDesVariablesEntieresSurCritere( PROBLEME_PNE * Pne ) 
{
int Var; int Var1; int * TypeDeBorneTrav; int * TypeDeVariableTrav; double * UminTrav; double * UmaxTrav;
double * LTrav; int * CdebTrav; int * CsuiTrav; double * ATrav; char  OnPeutFixer; double Ai; int Cnt;
char * SensContrainteTrav; int * NumContrainteTrav; int ic; int il; int ilMax; double S; double A;
double PlusGrandeBorneInf; double PlusPetiteBorneSup; int * MdebTrav; int * NbTermTrav; int * NuvarTrav; double * BTrav;
char OnPeutEssayerDeFixerAuMin; char OnPeutEssayerDeFixerAuMax; double * UTrav;

return; /* C'est pas concluant */

TypeDeBorneTrav = Pne->TypeDeBorneTrav;
UminTrav = Pne->UminTrav;
UmaxTrav = Pne->UmaxTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;
LTrav = Pne->LTrav;
CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
ATrav = Pne->ATrav;
SensContrainteTrav = Pne->SensContrainteTrav;
NumContrainteTrav = Pne->NumContrainteTrav;
MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;
NuvarTrav = Pne->NuvarTrav;
BTrav = Pne->BTrav;
UTrav = Pne->UTrav;

for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {

  if ( TypeDeVariableTrav[Var] != ENTIER ) continue;
  if ( Pne->TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;
	if ( UminTrav[Var] == UmaxTrav[Var] ) continue;
	
  OnPeutEssayerDeFixerAuMin = NON_PNE;
  OnPeutEssayerDeFixerAuMax = NON_PNE;
  if ( LTrav[Var] == 0.0 ) {
    OnPeutEssayerDeFixerAuMin = OUI_PNE;
    OnPeutEssayerDeFixerAuMax = OUI_PNE;	  
	}
	else if ( LTrav[Var] < 0.0 ) OnPeutEssayerDeFixerAuMax = OUI_PNE;
	else OnPeutEssayerDeFixerAuMin = OUI_PNE;

	/* 1- On cherche a la fixer sur la borne inf */
	if ( OnPeutEssayerDeFixerAuMin == OUI_PNE ) {
	  OnPeutFixer = OUI_PNE;
    PlusGrandeBorneInf = -LINFINI_PNE;
    ic = CdebTrav[Var];
    while ( ic >= 0 ) {
      Ai = ATrav[ic];
      if ( Ai == 0.0 ) goto ContrainteSuivante_1;
      Cnt = NumContrainteTrav[ic];
      if ( SensContrainteTrav[Cnt] == '=' ) { 
        OnPeutFixer = NON_PNE;
        break; 
      }
		  /* On calcule la plus grande borne inf donnee par les inequations ou Var a un coeff negatif */
		  S = 0.0;
      if ( Ai < 0.0 ) {
		    /* On cherche a maximiser la partie restante */
			  S = -BTrav[Cnt];
	      il = MdebTrav[Cnt];
	      ilMax = il + NbTermTrav[Cnt];
	      while ( il < ilMax ) {
		      Var1 = NuvarTrav[il];
				  A = ATrav[il];
				  if ( Var1 != Var ) {
            if ( A > 0.0 ) {
					    /* On prend la borne sup */
					    if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
						    S += A * UmaxTrav[Var1];						
						  }
					    else if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE || UminTrav[Var1] == UmaxTrav[Var1] ) S += A * UTrav[Var1];
						  else {
                OnPeutFixer = NON_PNE;
                break; 
						  }
					  }
					  else if (  A < 0.0 ) {
					    /* On prend la borne inf */
					    if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_INFERIEUREMENT ) {
						    S += A * UminTrav[Var1];						
						  }
					    else if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE || UminTrav[Var1] == UmaxTrav[Var1] ) S += A * UTrav[Var1];
						  else {
                OnPeutFixer = NON_PNE;
                break; 
						  }
					  }
          }
          il++;			
		    }
		    if ( OnPeutFixer == NON_PNE ) break;
        /* Calcul de la borne inf */
		    S /= fabs( Ai );
		    if ( S > PlusGrandeBorneInf ) PlusGrandeBorneInf = S;			
      }
		  ContrainteSuivante_1:
      ic = CsuiTrav[ic];
	  }
	  if ( OnPeutFixer == OUI_PNE ) {
	    /* On cherche a fixer la variable a UminTrav */
		  if ( PlusGrandeBorneInf <= UminTrav[Var] ) {
		    printf("Variable entiere %d fixee a %e  (Umin)\n",Var,UminTrav[Var]);
        UmaxTrav[Var] = UminTrav[Var];				
        continue;			
		  }
	  }
	}
	/* 2- On cherche a la fixer sur la borne sup */
	if ( OnPeutEssayerDeFixerAuMax == OUI_PNE ) {
	  OnPeutFixer = OUI_PNE;
    PlusPetiteBorneSup = LINFINI_PNE;
    ic = CdebTrav[Var];
    while ( ic >= 0 ) {
      Ai = ATrav[ic];
      if ( Ai == 0.0 ) goto ContrainteSuivante_2;
      Cnt = NumContrainteTrav[ic];
      if ( SensContrainteTrav[Cnt] == '=' ) { 
        OnPeutFixer = NON_PNE;
        break; 
      }
		  /* On calcule la plus petite borne sup donnee par les inequations ou Var a un coeff positif */
      if ( Ai > 0.0 ) {
		    /* On cherche a minimiser la partie restante */
			  S = BTrav[Cnt];
	      il = MdebTrav[Cnt];
	      ilMax = il + NbTermTrav[Cnt];
	      while ( il < ilMax ) {
		      Var1 = NuvarTrav[il];
				  if ( Var1 != Var ) {
				    A = -ATrav[il]; 
            if ( A > 0.0 ) {
					    /* On prend la borne inf */
					    if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_INFERIEUREMENT ) {
						    S += A * UminTrav[Var1];						
						  }
					    else if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE || UminTrav[Var1] == UmaxTrav[Var1] ) S += A * UTrav[Var1];
						  else {
                OnPeutFixer = NON_PNE;
                break; 
						  }
					  }
					  else if (  A < 0.0 ) {
					    /* On prend la borne sup */
					    if ( TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_DES_DEUX_COTES || TypeDeBorneTrav[Var1] == VARIABLE_BORNEE_SUPERIEUREMENT ) {
						    S += A * UmaxTrav[Var1];						
						  }
					    else if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE || UminTrav[Var1] == UmaxTrav[Var1] ) S += A * UTrav[Var1];
						  else {
                OnPeutFixer = NON_PNE;
                break; 
						  }
					  }
          }
          il++;			
		    }
		    if ( OnPeutFixer == NON_PNE ) break;
        /* Calcul de la borne inf */
		    S /= Ai;
		    if ( S < PlusPetiteBorneSup ) PlusPetiteBorneSup = S;			
      }
      ContrainteSuivante_2:		
     ic = CsuiTrav[ic];
	  }
	  if ( OnPeutFixer == OUI_PNE ) {
	    /* On cherche a fixer la variable a UminTrav */
		  if ( PlusPetiteBorneSup >= UmaxTrav[Var] ) {
		    printf("Variable entiere %d fixee a %e  (Umax)\n",Var,UmaxTrav[Var]);
        UminTrav[Var] = UmaxTrav[Var];							
		  }
	  }
	}
}

return;
}


