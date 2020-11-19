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

extern "C"
{
#include "spx_fonctions.h"
}





bool OPT_PilotageOptimisationQuadratique( PROBLEME_HEBDO * ProblemeHebdo )
{
int PdtHebdo;


if ( ProblemeHebdo->LeProblemeADejaEteInstancie == NON_ANTARES ) {
	
	OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique( ProblemeHebdo );
	
	OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique( ProblemeHebdo );

	ProblemeHebdo->LeProblemeADejaEteInstancie = OUI_ANTARES;
}


bool result = true;
if (ProblemeHebdo->NombreDInterconnexions > 0) {
	for ( PdtHebdo = 0; PdtHebdo < ProblemeHebdo->NombreDePasDeTemps; PdtHebdo++ ) {
		



		#ifdef dbgInfos
		printf("*********** Optimisation quadratique du pas de temps %ld ***********\n",PdtHebdo);
		#endif
		
		OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique( ProblemeHebdo, PdtHebdo );
		
		OPT_InitialiserLeSecondMembreDuProblemeQuadratique( ProblemeHebdo, PdtHebdo );
		
		OPT_InitialiserLesCoutsQuadratiques( ProblemeHebdo, PdtHebdo );
		
		
		result = OPT_AppelDuSolveurQuadratique( ProblemeHebdo->ProblemeAResoudre, PdtHebdo ) && result;
	}
}

return result;
}


