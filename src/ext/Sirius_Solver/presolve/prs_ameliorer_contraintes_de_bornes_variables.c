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

   FONCTION: Presolve, amelioration des coefficients des contraintes
             de type "variable upper/lower bound".
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

/*----------------------------------------------------------------------------*/

void PRS_AmeliorerCoeffDesContraintesDeBornesVariables( PRESOLVE * Presolve, int * NbModifications )
{		      
int Cnt; int il1; int il2; double A1; double A2; int Var1; int Var2; PROBLEME_PNE * Pne;
int NombreDeContraintes; char * SensContrainte; int * Mdeb; int * NbTerm;
int * Nuvar; double * A; double * B; int * TypeDeBornePourPresolve;
int * TypeDeVariable; double * BorneInfPourPresolve; double * BorneSupPourPresolve;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

NombreDeContraintes = Pne->NombreDeContraintesTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;
TypeDeVariable = Pne->TypeDeVariableTrav;

TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
BorneInfPourPresolve = Presolve->BorneInfPourPresolve;
BorneSupPourPresolve = Presolve->BorneSupPourPresolve;

*NbModifications = 0;

for ( Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {
  if ( SensContrainte[Cnt] != '<' ) continue;
  if ( NbTerm[Cnt] != 2 ) continue;
  if ( B[Cnt] != 0.0 ) continue;
  il1    = Mdeb[Cnt];
  Var1 = Nuvar[il1];
  A1   = A[il1];  
  if ( A1 == 0.0 ) continue;
  if ( TypeDeBornePourPresolve[Var1] == VARIABLE_FIXE ) continue;  
  il2 = il1 + 1;
  Var2 = Nuvar[il2];
  A2   = A[il2];  
  if ( A2 == 0.0 ) continue;
  if ( TypeDeBornePourPresolve[Var2] == VARIABLE_FIXE ) continue;  
  
  if ( TypeDeVariable[Var1] != ENTIER && TypeDeVariable[Var2] == ENTIER ) {
    if ( A1 > 0.0 ) {
      A2/= A1;
      A1 = 1.0;
      #if VERBOSE_PRS
        /*printf("A1 creel= %e entier= %e  lim %e\n",A1,A2,Pne->UmaxTrav[Var1]);*/
      #endif
      if ( -A2 > BorneSupPourPresolve[Var1] ) {
        #if VERBOSE_PRS
	        /*printf("possibilite %e -> %e\n",-A2,-Pne->UmaxTrav[Var1]);*/
        #endif
	      A[il1] = 1.0;
	      A[il2] = -BorneSupPourPresolve[Var1];
        *NbModifications = *NbModifications + 1;	
      }
    }
    else if ( fabs( A1 ) > ZERO_PRESOLVE ) {
      A2/= fabs( A1 );
      A1 = -1.0;
      #if VERBOSE_PRS
        /*printf("A1 creel= %e centier= %e  lim %e\n",A1,A2,Pne->UminTrav[Var1]);*/
      #endif
      if ( A2 < BorneInfPourPresolve[Var1] ) {
        #if VERBOSE_PRS
	        /*printf("possibilite %e -> %e\n",A2,Pne->UminTrav[Var1]);*/
        #endif
	      A[il1] = -1.0;
	      A[il2] = BorneInfPourPresolve[Var1];
        *NbModifications = *NbModifications + 1;	
      }
    }
  }
  else if ( TypeDeVariable[Var1] == ENTIER && TypeDeVariable[Var2] != ENTIER ) {
    if ( A2 > 0.0 ) {
      A1/= A2;
      A2 = 1.0;      
      #if VERBOSE_PRS
        /*printf("A2 creel= %e centier= %e  lim %e\n",A2,A1,Pne->UmaxTrav[Var2]);*/
      #endif
      if ( -A1 > BorneSupPourPresolve[Var2] ) {
        #if VERBOSE_PRS
          /*printf("possibilite %e -> %e\n",-A1,-Pne->UmaxTrav[Var2]);*/
        #endif
        A[il1] = -BorneSupPourPresolve[Var2];
        A[il2] = 1.0;
        *NbModifications = *NbModifications + 1;	
      }
    }
    else if ( fabs( A2 ) > ZERO_PRESOLVE ){
      A1/= fabs( A2 );
      A2 = -1.0;      
      #if VERBOSE_PRS
        /*printf("A2 creel= %e centier= %e  lim %e\n",A2,A1,Pne->UminTrav[Var2]);*/
      #endif
      if ( A1 < BorneInfPourPresolve[Var2] ) {
        #if VERBOSE_PRS
          /*printf("possibilite %e -> %e\n",A1,Pne->UminTrav[Var2]);*/
        #endif
        A[il1] = BorneInfPourPresolve[Var2];
        A[il2] = -1.0;
        *NbModifications = *NbModifications + 1;	
      }
    }
  }
    
}

return;
}
 
