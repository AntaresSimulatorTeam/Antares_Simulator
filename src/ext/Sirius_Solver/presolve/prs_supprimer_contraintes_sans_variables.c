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

   FONCTION: 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_fonctions.h"
# include "pne_define.h"

/*----------------------------------------------------------------------------*/
/* Attention, il faut verifier que la contrainte avec que des variables 
   fixees est satisfaite, c'est pour ca qu'on appelle ce SP apres avoir 
   calcule les bornes sur les contraintes                                     */

void PRS_SupprimerLesContraintesAvecQueDesVariablesFixees( PRESOLVE * Presolve )

{
int il; int ilMax; int Cnt; int QueDesVarFix; int Nb; double S; char ControleActive;
int NombreDeContraintes; char * ContrainteInactive; int * Mdeb; int * NbTerm;
int * Nuvar; int * TypeDeBornePourPresolve; double * A; double * ValeurDeXPourPresolve;
char * SensContrainte; double * B; PROBLEME_PNE * Pne;

Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;

if ( Pne->YaDesVariablesEntieres == NON_PNE ) {
  /* Si on est en continu, on ne sait pas (pour l'instant) recalculer exactement les variables
	   duales des contraintes quand on fait des substitutions de variables. Donc on prefere ne pas
		 faire ce genre de presolve. Todo: stocker toutes les transfromations de la matrice pour
		 recalculer exactement les variables duales. */
	return;
}  

NombreDeContraintes = Pne->NombreDeContraintesTrav;
Mdeb = Pne->MdebTrav;
NbTerm = Pne->NbTermTrav;
SensContrainte = Pne->SensContrainteTrav;
B = Pne->BTrav;
Nuvar = Pne->NuvarTrav;
A = Pne->ATrav;

ValeurDeXPourPresolve = Presolve->ValeurDeXPourPresolve;
TypeDeBornePourPresolve = Presolve->TypeDeBornePourPresolve;
ContrainteInactive = Presolve->ContrainteInactive;

ControleActive = NON_PNE;

for ( Nb = 0 , Cnt = 0 ; Cnt < NombreDeContraintes ; Cnt++ ) {

  if ( ContrainteInactive[Cnt] == OUI_PNE ) continue;

  QueDesVarFix = OUI_PNE;

  S = 0.;

  il    = Mdeb[Cnt];
  ilMax = il + NbTerm[Cnt];
  while ( il < ilMax ) {
    if ( TypeDeBornePourPresolve[Nuvar[il]] != VARIABLE_FIXE ) {
      QueDesVarFix = NON_PNE;
      break; 
    }
    S+= A[il] * ValeurDeXPourPresolve[Nuvar[il]];		
    il++;
  }
  if ( QueDesVarFix == OUI_PNE ) {
    Nb++;
    /*
    printf("contrainte %d avec QueDesVarFix\n",Cnt);
    */
    PRS_DesactiverContrainte( Presolve, Cnt );
		
    /* On verifie que la contrainte est satisfaite */
    if ( SensContrainte[Cnt] == '=' ) {
      if ( fabs ( S - B[Cnt] ) > SEUIL_DADMISSIBILITE && ControleActive == OUI_PNE ) { 
			  # if VERBOSE_PRS == 1
          printf("*** Phase de Presolve-> La contrainte %d ne comporte que des variables fixees dont les valeurs\n",Cnt);
          printf("sont incompatibles avec la valeur du second membre.\n");
        # endif
				Pne->YaUneSolution = PROBLEME_INFAISABLE;
        return;
      }
    } 
    else {
      /* La contrainte est de type <= */
      if ( S > B[Cnt] + SEUIL_DADMISSIBILITE && ControleActive == OUI_PNE ) { 
			  # if VERBOSE_PRS == 1
          printf("*** Phase de Presolve-> La contrainte %d ne comporte que des variables fixees dont les valeurs\n",Cnt);
          printf("sont incompatibles avec la valeur du second membre.\n");
        # endif
        Pne->YaUneSolution = PROBLEME_INFAISABLE;
        return;
      }
    }
  }

}

#if VERBOSE_PRS
  printf("-> nombre de contraintes supprimees car contenant uniquement des variables fixees %d\n",Nb); 
#endif

return;
}
 

