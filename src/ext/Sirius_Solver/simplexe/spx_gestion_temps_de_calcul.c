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

   FONCTION: Gestion des temps de calcul
               
   AUTEUR: R. GONZALEZ

************************************************************************/

# include "spx_sys.h"

# include "spx_fonctions.h"
# include "spx_define.h"


/*----------------------------------------------------------------------------*/

void SPX_InitDateDebutDuCalcul( PROBLEME_SPX * Spx )
{
time_t HeureDeCalendrierDebut;

time( &HeureDeCalendrierDebut );
Spx->HeureDeCalendrierDebut = HeureDeCalendrierDebut;

return;
}

/*----------------------------------------------------------------------------*/

void SPX_ControleDuTempsEcoule( PROBLEME_SPX * Spx )
{
time_t HeureDeCalendrierCourant;
double TempsEcoule;

if ( Spx->DureeMaxDuCalcul < 0 ) return;

time( &HeureDeCalendrierCourant );

TempsEcoule = difftime( HeureDeCalendrierCourant , Spx->HeureDeCalendrierDebut );

if ( TempsEcoule <= 0.0 ) TempsEcoule = 0.0;

/* On provoque l'arret du calcul si temps depasse */
if ( TempsEcoule > Spx->DureeMaxDuCalcul ) Spx->Iteration = 10 * Spx->NombreMaxDIterations;

return;
}
