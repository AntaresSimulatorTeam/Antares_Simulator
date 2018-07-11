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



void OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage( PROBLEME_HEBDO * ProblemeHebdo )  
{
int Pays; int Index; int PdtHebdo; int NombreDePasDeTempsProblemeHebdo;
	
PALIERS_THERMIQUES * PaliersThermiquesDuPays; PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout;
double TailleUnitaireDUnGroupeDuPalierThermique; double * PuissanceMinDuPalierThermique_SV;
int * NombreMaxDeGroupesEnMarcheDuPalierThermique; int * NombreMinDeGroupesEnMarcheDuPalierThermique; 
double * PuissanceDisponibleDuPalierThermiqueRef_SV;


if ( ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == NON_ANTARES ) return;
                                                                                                                                                                                                                                                                                   
NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTemps;

for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
	PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
	PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;
	
	for ( Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++ ) {			
	  PuissanceDisponibleDuPalierThermiqueRef_SV  = PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermiqueRef_SV;  
		PuissanceMinDuPalierThermique_SV            = PuissanceDisponibleEtCout[Index]->PuissanceMinDuPalierThermique_SV;
		NombreMaxDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMaxDeGroupesEnMarcheDuPalierThermique;
		NombreMinDeGroupesEnMarcheDuPalierThermique = PuissanceDisponibleEtCout[Index]->NombreMinDeGroupesEnMarcheDuPalierThermique;

	  TailleUnitaireDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique[Index];
		
		for ( PdtHebdo = 0 ; PdtHebdo < NombreDePasDeTempsProblemeHebdo ; PdtHebdo++ ) {
			if ( TailleUnitaireDUnGroupeDuPalierThermique != 0 ) {						
		    NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = (int) ceil( PuissanceDisponibleDuPalierThermiqueRef_SV[PdtHebdo] / TailleUnitaireDUnGroupeDuPalierThermique );
			  
			  NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = (int) ceil( PuissanceMinDuPalierThermique_SV[PdtHebdo] / TailleUnitaireDUnGroupeDuPalierThermique );
			}
			else {
		    NombreMaxDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = 0;
			  
			  NombreMinDeGroupesEnMarcheDuPalierThermique[PdtHebdo] = 0;			
			}			
		}
	}
}

return;
}
