/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/






#include <math.h>
#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include <antares/emergency.h>
#include <antares/logs.h>

extern "C"
{
#include "../ext/Sirius_Solver/simplexe/spx_fonctions.h"
}

using namespace Antares;



void OPT_LiberationProblemesSimplexe( PROBLEME_HEBDO * ProblemeHebdo )
{
int i; PROBLEME_SPX * ProbSpx; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;
int NbIntervalles; int NumIntervalle; int NombreDePasDeTempsPourUneOptimisation;

if ( ProblemeHebdo->OptimisationAuPasHebdomadaire == NON_ANTARES ) {
	NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsDUneJournee;
}
else {
	NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTemps;
}
NbIntervalles = (int) (ProblemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation );

ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
if (ProblemeAResoudre) {
	
	if ( ProblemeHebdo->LeProblemeADejaEteInstancie == NON_ANTARES ) {
		
		for ( i = 0 ; i < ProblemeHebdo->NombreDeClassesDeManoeuvrabiliteActives ; i++ ) {
			for ( NumIntervalle = 0 ; NumIntervalle < NbIntervalles ; NumIntervalle++ ) {
				ProbSpx = (PROBLEME_SPX *) ((ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[i])->ProblemeSpx[NumIntervalle]);
				if ( ProbSpx != NULL ) {
					SPX_LibererProbleme( ProbSpx );
					(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[i])->ProblemeSpx[NumIntervalle] = NULL;
				}
			}
		}
	}
}

return;
}


