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
#include "../simulation/sim_structure_probleme_adequation.h"



void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise( PROBLEME_HEBDO * ProblemeHebdo )
{
	int NombreDePasDeTempsPourUneOptimisation; int NbIntervallesOptimises; int NombreDeJoursParIntervalle;
	int Jour; int i; int j; int Pays;
	double CntTurbParIntervalle;
	double MaxPompageParIntervalle;
	double CntMinEParIntervalle;
	double CntMaxEParIntervalle;
	double * CntEnergieH2OParIntervalleOptimise;
	double * CntEnergieH2OParIntervalleOptimiseRef;
	double * CntEnergieH2OParJour;
	double InflowSum;	

	NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation; 

	NbIntervallesOptimises = (int) ( ProblemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation );

	NombreDeJoursParIntervalle = (int) ( NombreDePasDeTempsPourUneOptimisation / ProblemeHebdo->NombreDePasDeTempsDUneJournee );

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		char presenceHydro = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
		char TurbEntreBornes = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
		if (presenceHydro == OUI_ANTARES && TurbEntreBornes == NON_ANTARES)
		{
			CntEnergieH2OParIntervalleOptimise = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->CntEnergieH2OParIntervalleOptimise;
			CntEnergieH2OParIntervalleOptimiseRef = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->CntEnergieH2OParIntervalleOptimiseRef;
			CntEnergieH2OParJour = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->CntEnergieH2OParJour;
			
			
			Jour = 0;
			for (i = 0; i < NbIntervallesOptimises; i++) {
				CntTurbParIntervalle = 0.0;
				MaxPompageParIntervalle = 0.;
				for (j = 0; j < NombreDeJoursParIntervalle; j++, Jour++)
				{
					CntTurbParIntervalle += CntEnergieH2OParIntervalleOptimise[Jour];

					
					CntEnergieH2OParJour[Jour] = CntEnergieH2OParIntervalleOptimise[Jour];
				}

				
				
				CntEnergieH2OParIntervalleOptimise[i] = CntTurbParIntervalle;
				CntEnergieH2OParIntervalleOptimiseRef[i] = CntTurbParIntervalle;
			}
		}
	}


	
	
	double * MinEnergieHydrauParIntervalleOptimise;
	double * MaxEnergieHydrauParIntervalleOptimise;

	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++) {
		char presenceHydro = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable;
		char TurbEntreBornes = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
		if (presenceHydro == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES)
		{
			MinEnergieHydrauParIntervalleOptimise = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->MinEnergieHydrauParIntervalleOptimise;
			MaxEnergieHydrauParIntervalleOptimise = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxEnergieHydrauParIntervalleOptimise;
			CntEnergieH2OParJour = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->CntEnergieH2OParJour;
			Jour = 0;
			for (i = 0; i < NbIntervallesOptimises; i++) {
				CntMinEParIntervalle = 0.;
				CntMaxEParIntervalle = 0.;
				for (j = 0; j < NombreDeJoursParIntervalle; j++, Jour++) {
					CntMinEParIntervalle += MinEnergieHydrauParIntervalleOptimise[Jour];
					CntMaxEParIntervalle += MaxEnergieHydrauParIntervalleOptimise[Jour];

					
					CntEnergieH2OParJour[Jour] = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxEnergieHydrauParIntervalleOptimise[Jour];
				}
				MinEnergieHydrauParIntervalleOptimise[i] = CntMinEParIntervalle;
				MaxEnergieHydrauParIntervalleOptimise[i] = CntMaxEParIntervalle;
			}
		}
	}


	
	double * MaxEnergiePompageParIntervalleOptimise;

	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
	{
		MaxEnergiePompageParIntervalleOptimise = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->MaxEnergiePompageParIntervalleOptimise;
		
		Jour = 0;
		for (i = 0; i < NbIntervallesOptimises; i++) {
			CntTurbParIntervalle = 0.0;
			MaxPompageParIntervalle = 0.;
			for (j = 0; j < NombreDeJoursParIntervalle; j++, Jour++)
				MaxPompageParIntervalle += MaxEnergiePompageParIntervalleOptimise[Jour];

			MaxEnergiePompageParIntervalleOptimise[i] = MaxPompageParIntervalle;
		}
	}
	
	double * InflowForTimeInterval;

	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
	{
		InflowForTimeInterval = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->InflowForTimeInterval;
		Jour = 0;
		for (i = 0; i < NbIntervallesOptimises; i++) {
			
			InflowSum = 0.;
			for (j = 0; j < NombreDeJoursParIntervalle; j++, Jour++)
				InflowSum += InflowForTimeInterval[Jour];

			InflowForTimeInterval[i] = InflowSum;
		}
	}
	
	return;
}
