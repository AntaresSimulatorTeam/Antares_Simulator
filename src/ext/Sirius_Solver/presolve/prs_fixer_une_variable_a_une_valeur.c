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

   FONCTION: Init des indicateurs de fixation d'une variable.					 
                
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "prs_sys.h"

# include "prs_fonctions.h"
# include "prs_define.h"

# include "pne_define.h"
		
# ifdef PRS_UTILISER_LES_OUTILS_DE_GESTION_MEMOIRE_PROPRIETAIRE	
  # include "prs_memoire.h"
# endif

/*----------------------------------------------------------------------------*/

void PRS_FixerUneVariableAUneValeur( PRESOLVE * Presolve, int Var, double X )   
{
PROBLEME_PNE * Pne;
Pne = (PROBLEME_PNE *) Presolve->ProblemePneDuPresolve;
Pne->TypeDeVariableTrav[Var] = REEL;
Presolve->ValeurDeXPourPresolve[Var] = X;
/* Car on se sert de ces bornes dans le postsolve 
Presolve->BorneInfPourPresolve[Var] = X;
Presolve->BorneSupPourPresolve[Var] = X;							
*/
Presolve->TypeDeBornePourPresolve[Var] = VARIABLE_FIXE;
Presolve->TypeDeValeurDeBorneInf[Var] = VALEUR_IMPLICITE;
Presolve->TypeDeValeurDeBorneSup[Var] = VALEUR_IMPLICITE;
return;
}

/*----------------------------------------------------------------------------*/
