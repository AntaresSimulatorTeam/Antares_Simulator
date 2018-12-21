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



























# include "opt_structure_probleme_a_resoudre.h"

# include "../simulation/simulation.h"
# include "../simulation/sim_structure_donnees.h"
# include "../simulation/sim_extern_variables_globales.h"
 
# include "opt_fonctions.h"

# include "../ext/Sirius_Solver/simplexe/spx_constantes_externes.h"
# include "../simulation/sim_structure_probleme_adequation.h"

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/emergency.h>
using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;




void RenseignerDonneesCoutsDemarrage( PROBLEME_HEBDO * ProblemeHebdo )
{
int Pays; int Index; PALIERS_THERMIQUES * PaliersThermiquesDuPays; double X; char Flag;

if ( ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == NON_ANTARES ) return;

srand(1);

for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
	PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
	
	for ( Index = 0 ; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Index++ ) {			
	  
    PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index] = 10 *
		  fabs(PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]->CoutHoraireDeProductionDuPalierThermiqueRef[1]) *
			(0.5 * PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermique[1]+1);


		if ( PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index] > 5.e+4 ) {
		  PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index] = 5.e+4;
		}
			
    PaliersThermiquesDuPays->CoutDArretDUnGroupeDuPalierThermique[Index] = 0.;	
		
	  
    PaliersThermiquesDuPays->CoutFixeDeMarcheDUnGroupeDuPalierThermique[Index] = 0.1 * 
		  fabs(PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]->CoutHoraireDeProductionDuPalierThermiqueRef[1]) *
			(1.0 * PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermique[1]+1);		

		PaliersThermiquesDuPays->CoutFixeDeMarcheDUnGroupeDuPalierThermique[Index] = 0.001 * PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index];



Flag = 0;
if ( PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Index] > 2.e+3 ) {
  printf("RenseignerDonneesCoutsDemarrage: Pays %d Index %d TailleUnitaireDUnGroupeDuPalierThermique %e\n",Pays,Index,PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Index]);
  PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Index] = 2.e+3;
	Flag = 1;
}


		
    PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[Index] = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Index];

    PaliersThermiquesDuPays->PminDUnGroupeDuPalierThermique[Index] = 0.1 * PaliersThermiquesDuPays->PmaxDUnGroupeDuPalierThermique[Index];

		
    X = rand();
		X /= RAND_MAX;
		X *= 48;
		X += 2; 

    PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[Index] = (int) X;

		X *= 0.51;		

    PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index] = (int) X;

if ( Flag == 1 ) {
  PaliersThermiquesDuPays->CoutDeDemarrageDUnGroupeDuPalierThermique[Index] = 0;
  PaliersThermiquesDuPays->CoutDArretDUnGroupeDuPalierThermique[Index] = 0.;	
  PaliersThermiquesDuPays->CoutFixeDeMarcheDUnGroupeDuPalierThermique[Index] = 0.;
  PaliersThermiquesDuPays->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[Index] = 1;
  PaliersThermiquesDuPays->DureeMinimaleDArretDUnGroupeDuPalierThermique[Index] = 1;	
}
		
		
	}
}
	
return;
}
