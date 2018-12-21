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

#include <antares/logs.h>
#include <antares/emergency.h>

using namespace Antares;





void OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser( PROBLEME_HEBDO * ProblemeHebdo, int * MxPalierThermique )
{
	int Pays; int MxPaliers; int NombreDePasDeTempsPourUneOptimisation; int CntCouplante;
	int NombreDeJoursDansUnIntervalleOptimise; char ContrainteDeReserveJMoins1ParZone;
	PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;

	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;
	ContrainteDeReserveJMoins1ParZone = ProblemeHebdo->ContrainteDeReserveJMoins1ParZone;
	
	NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation; 

	MxPaliers = 0;
	ProblemeAResoudre->NombreDeVariables = ProblemeHebdo->NombreDInterconnexions;
	
	if ( COUT_TRANSPORT == OUI_ANTARES ) ProblemeAResoudre->NombreDeVariables+= 2 * ProblemeHebdo->NombreDInterconnexions;

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		ProblemeAResoudre->NombreDeVariables+= ProblemeHebdo->PaliersThermiquesDuPays[Pays]->NombreDePaliersThermiques;
		



		MxPaliers += ProblemeHebdo->PaliersThermiquesDuPays[Pays]->NombreDePaliersThermiques;

		
		if ( ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable == OUI_ANTARES ) {
			ProblemeAResoudre->NombreDeVariables++;
		}
		
		if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable == OUI_ANTARES) {
			ProblemeAResoudre->NombreDeVariables++;
		}
		
		if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire == OUI_ANTARES) {
			ProblemeAResoudre->NombreDeVariables++; 
			ProblemeAResoudre->NombreDeVariables++; 
		}

		ProblemeAResoudre->NombreDeVariables+= 2; 

		if ( ContrainteDeReserveJMoins1ParZone == OUI_ANTARES ) {
			ProblemeAResoudre->NombreDeVariables+= 1; 
		}
		
	}
	ProblemeAResoudre->NombreDeVariables *= NombreDePasDeTempsPourUneOptimisation;

	ProblemeAResoudre->NombreDeContraintes = ProblemeHebdo->NombreDePays;   

	ProblemeAResoudre->NombreDeContraintes += ProblemeHebdo->NombreDePays;  
	
	
	if ( COUT_TRANSPORT == OUI_ANTARES ) ProblemeAResoudre->NombreDeContraintes+= ProblemeHebdo->NombreDInterconnexions;               

	if ( ContrainteDeReserveJMoins1ParZone == OUI_ANTARES ) {
		
		ProblemeAResoudre->NombreDeContraintes += 2 * ProblemeHebdo->NombreDeZonesDeReserveJMoins1;
	}

	
	for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
		if ( ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE ) {
			ProblemeAResoudre->NombreDeContraintes++;
		}
	}	
	ProblemeAResoudre->NombreDeContraintes *= NombreDePasDeTempsPourUneOptimisation;

	
	if ( NombreDePasDeTempsPourUneOptimisation > ProblemeHebdo->NombreDePasDeTempsDUneJournee ) {
		NombreDeJoursDansUnIntervalleOptimise	= ProblemeHebdo->NombreDePasDeTemps / ProblemeHebdo->NombreDePasDeTempsDUneJournee;
	}
	else NombreDeJoursDansUnIntervalleOptimise = 1;
	
	for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
		if ( ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]->TypeDeContrainteCouplante == CONTRAINTE_JOURNALIERE ) {
			ProblemeAResoudre->NombreDeContraintes+= NombreDeJoursDansUnIntervalleOptimise;
		}
	}	

	if ( NombreDePasDeTempsPourUneOptimisation > ProblemeHebdo->NombreDePasDeTempsDUneJournee ) {
		for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
			if ( ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante]->TypeDeContrainteCouplante == CONTRAINTE_HEBDOMADAIRE ) {
				ProblemeAResoudre->NombreDeContraintes++;
			}
		}
	}


	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
	{
		
		char Pump = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDePompageModulable;
		char TurbEntreBornes = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->TurbinageEntreBornes;
		char MonitorHourlyLev = ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->SuiviNiveauHoraire;

		
		if (Pump == NON_ANTARES && TurbEntreBornes == NON_ANTARES && MonitorHourlyLev == NON_ANTARES)
		{
			
			if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable == OUI_ANTARES) {
				ProblemeAResoudre->NombreDeContraintes++;
			}
		}

		
		if (Pump == OUI_ANTARES && TurbEntreBornes == NON_ANTARES && MonitorHourlyLev == NON_ANTARES)
		{
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
		}

		
		if (Pump == NON_ANTARES && TurbEntreBornes == OUI_ANTARES && MonitorHourlyLev == NON_ANTARES)
		{
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
		}

		
		if (Pump == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES && MonitorHourlyLev == NON_ANTARES)
		{
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
		}

		
		if (Pump == NON_ANTARES && TurbEntreBornes == OUI_ANTARES && MonitorHourlyLev == OUI_ANTARES)
		{
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
		}

		
		if (Pump == OUI_ANTARES && TurbEntreBornes == OUI_ANTARES && MonitorHourlyLev == OUI_ANTARES)
		{
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes++;
			
			ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
		}

		
		if (Pump == OUI_ANTARES && TurbEntreBornes == NON_ANTARES && MonitorHourlyLev == OUI_ANTARES)
		{
			logs.fatal() << "Level explicit modeling requires flexible generation";
			AntaresSolverEmergencyShutdown();
		}

		
		if (Pump == NON_ANTARES && TurbEntreBornes == NON_ANTARES && MonitorHourlyLev == OUI_ANTARES)
		{
			logs.fatal() << "Level explicit modeling requires flexible generation";
			AntaresSolverEmergencyShutdown();
		}
	}

	
	
	for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
	{
		if (ProblemeHebdo->CaracteristiquesHydrauliques[Pays]->PresenceDHydrauliqueModulable == OUI_ANTARES)
		{
			if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
			{
				
				ProblemeAResoudre->NombreDeVariables += NombreDePasDeTempsPourUneOptimisation * 2;
				ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation;
			}

			else if (ProblemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
			{
				
				ProblemeAResoudre->NombreDeVariables += 2;
				ProblemeAResoudre->NombreDeContraintes += NombreDePasDeTempsPourUneOptimisation * 2;
			}
		}
	}
	*MxPalierThermique = MxPaliers;

	if ( ProblemeHebdo->OptimisationAvecCoutsDeDemarrage == OUI_ANTARES ) {
		OPT_DecompteDesVariablesEtDesContraintesCoutsDeDemarrage( ProblemeHebdo );  
	}	

	return;
}



