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

#include <yuni/io/file.h>
#include "opt_fonctions.h"

#define ZERO 1.e-2





void OPT_InitialiserLesPminHebdo( PROBLEME_HEBDO * ProblemeHebdo )
{
	int Pays; int Palier; int Pdt; PALIERS_THERMIQUES * PaliersThermiquesDuPays;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout;
  double * PuissanceMinDuPalierThermique; double * PuissanceMinDuPalierThermique_SV;
	int NombreDePasDeTempsProblemeHebdo;



	
	

  NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTempsRef;
	
	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {	
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;
		for ( Palier = 0 ; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Palier++ ) {
		  PuissanceMinDuPalierThermique    = PuissanceDisponibleEtCout[Palier]->PuissanceMinDuPalierThermique;
			PuissanceMinDuPalierThermique_SV = PuissanceDisponibleEtCout[Palier]->PuissanceMinDuPalierThermique_SV;
			
			for ( Pdt = 0 ; Pdt < NombreDePasDeTempsProblemeHebdo ; Pdt++ ) {
			  

			


				
				
				
				PuissanceMinDuPalierThermique_SV[Pdt] = PuissanceMinDuPalierThermique[Pdt];
				
			}
		}
	}
	
	return;
}




void OPT_CalculMaxPminJour( PROBLEME_HEBDO * ProblemeHebdo ,
                            PALIERS_THERMIQUES * PaliersThermiquesDuPays,
														PRODUCTION_THERMIQUE_OPTIMALE ** ProductionThermiqueOptimale,
														double * TailleUnitaireDUnGroupeDuPalierThermique,
														int     Palier,
                            double * MaxPminJour )														
{
int j  ; int NombreDeJoursProblemeHebdo      ; int PdtHebdo; double Pmin ; int Jour     ;
double P; int * NumeroDeJourDuPasDeTemps      ; double NbGroupesEquivalents                ;
int NombreDePasDeTempsProblemeHebdo           ; double * PminDuPalierThermiquePendantUnJour;
double * PminDuPalierThermiquePendantUneSemaine;

NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTemps;
NombreDeJoursProblemeHebdo      = ProblemeHebdo->NombreDeJours;
NumeroDeJourDuPasDeTemps        = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

PminDuPalierThermiquePendantUnJour       = PaliersThermiquesDuPays->PminDuPalierThermiquePendantUnJour;		
PminDuPalierThermiquePendantUneSemaine   = PaliersThermiquesDuPays->PminDuPalierThermiquePendantUneSemaine;

for ( j = 0; j < NombreDeJoursProblemeHebdo ; j++ ) MaxPminJour[j] = 0.0;

for ( PdtHebdo = 0; PdtHebdo < NombreDePasDeTempsProblemeHebdo ; PdtHebdo++ ) {
  P = ProductionThermiqueOptimale[PdtHebdo]->ProductionThermiqueDuPalier[Palier];
	
  if ( fabs( P ) <= ZERO ) continue;
  
  Jour = NumeroDeJourDuPasDeTemps[PdtHebdo];
	if ( TailleUnitaireDUnGroupeDuPalierThermique[Palier] > 0 ) {
	  NbGroupesEquivalents = P / TailleUnitaireDUnGroupeDuPalierThermique[Palier];
	}
	else {
		NbGroupesEquivalents = P;
	}
	
	NbGroupesEquivalents = ceil( NbGroupesEquivalents );
	
	Pmin = PminDuPalierThermiquePendantUnJour[Palier] * NbGroupesEquivalents;            
	if ( Pmin > MaxPminJour[Jour] ) MaxPminJour[Jour] = Pmin;
  
  Pmin = PminDuPalierThermiquePendantUneSemaine[Palier] * NbGroupesEquivalents;
	for ( j = 0 ; j < NombreDeJoursProblemeHebdo ; j++ ) {
    if ( Pmin > MaxPminJour[j] ) MaxPminJour[j] = Pmin;
	}
}

return;
}



void OPT_RestaurerLesPminThermiques( PROBLEME_HEBDO * ProblemeHebdo )  
{
	int Pays; int Index; double * PuissanceMinDuPalierThermique;  
	double * PuissanceMinDuPalierThermique_SV;
	
	PALIERS_THERMIQUES * PaliersThermiquesDuPays;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER * PuissanceDisponibleEtCout;

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		for (Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++) {
			PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Index];
			PuissanceMinDuPalierThermique    = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique;
			PuissanceMinDuPalierThermique_SV = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique_SV;					
			memcpy( (char *) PuissanceMinDuPalierThermique, (char *) PuissanceMinDuPalierThermique_SV,
							ProblemeHebdo->NombreDePasDeTempsRef * sizeof( double ) );
			
		}
	}
	return;
}


