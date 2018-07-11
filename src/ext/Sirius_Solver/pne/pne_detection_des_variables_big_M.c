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

   FONCTION: Marquage des contraintes mixtes sur lesquelles on peut
	           faire des MIR.
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "pne_sys.h"

# include "pne_fonctions.h"
# include "pne_define.h"

# ifdef PNE_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "pne_memoire.h"
# endif     

# define Ai_BIG_M        1.e+4
# define RAPPORT_BIG_M   5.

/*----------------------------------------------------------------------------*/

void PNE_DetecterLesVariablesBigM( PROBLEME_PNE * Pne )
{
int Cnt; int il; int ilMax; int Var; int Var1; int ic; int * MdebTrav;
int * NbTermTrav; int * NuvarTrav; int * TypeDeBorneTrav; double * ATrav;
int * TypeDeVariableTrav; double AmaxCont; double Ai; char * VariableBinaireBigM;
int * CdebTrav; int * CsuiTrav; int * NumContrainteTrav; double Abin;

MdebTrav = Pne->MdebTrav;
NbTermTrav = Pne->NbTermTrav;   
NuvarTrav = Pne->NuvarTrav;
ATrav = Pne->ATrav;
TypeDeBorneTrav = Pne->TypeDeBorneTrav;
TypeDeVariableTrav = Pne->TypeDeVariableTrav;

VariableBinaireBigM = Pne->VariableBinaireBigM;

if ( Pne->ChainageTransposeeExploitable == NON_PNE ) PNE_ConstruireLeChainageDeLaTransposee( Pne );

CdebTrav = Pne->CdebTrav;
CsuiTrav = Pne->CsuiTrav;
NumContrainteTrav = Pne->NumContrainteTrav;

Pne->YaDesBigM = NON_PNE;  
for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) VariableBinaireBigM[Var] = NON_PNE;

/* Le critere de detection des Big M se fait en analysant les rapports des coefficients dans les contraintes */

for ( Var = 0 ; Var < Pne->NombreDeVariablesTrav ; Var++ ) {
  if ( TypeDeVariableTrav[Var] != ENTIER ) continue;
  if ( TypeDeBorneTrav[Var] == VARIABLE_FIXE ) continue;
	ic = CdebTrav[Var];
  while ( ic >= 0 ) {
	  Cnt = NumContrainteTrav[ic];
	  il = MdebTrav[Cnt];
    ilMax = il + NbTermTrav[Cnt];		
		Abin = 0.;
		AmaxCont = -1.;
    while ( il < ilMax ) {
		  Var1 = NuvarTrav[il];
			Ai = fabs( ATrav[il] );
			if ( Var1 == Var ) {
				if ( Ai < Ai_BIG_M ) goto NextIc;
			  Abin = Ai;
			}
			else {
			  if ( TypeDeBorneTrav[Var1] == VARIABLE_FIXE ) goto NextIl;
				if ( Ai > AmaxCont ) AmaxCont = Ai;
			}
			NextIl:
		  il++;
		}
		if ( AmaxCont <= 0.0 ) goto NextIc;
		if ( fabs( Abin / AmaxCont ) < RAPPORT_BIG_M ) goto NextIc;
		Pne->YaDesBigM = OUI_PNE;
    VariableBinaireBigM[Var] = OUI_PNE;
		break;
	}
	NextIc:
	ic = CsuiTrav[ic];
}

if ( Pne->YaDesBigM == OUI_PNE ) {
  if ( Pne->AffichageDesTraces == OUI_PNE ) {
    printf("Big M detected for integer variables\n");
  }
}

return;
}
