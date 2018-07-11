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




void OPT_AjusterLesPmaxThermiques( PROBLEME_HEBDO * ProblemeHebdo, CLASSE_DE_MANOEUVRABILITE ClasseDeManoeuvrabilite )  
{
	int Pays; int Index   ; int PdtHebdo     ; double P   ; int PdtHeure; double NbGroupesEquivalents;
	int LongueurDuPasDeTemps; int i            ; double Pmin; int Jour    ;	double * MaxPminJour      ; 
  int NombreDePasDeTempsProblemeHebdo         ;	double * PuissanceDisponibleDuPalierThermiqueRef      ;
  int * NumeroDeJourDuPasDeTemps              ; double * TailleUnitaireDUnGroupeDuPalierThermique      ;
	double * PminDuPalierThermiquePendantUneHeure; double * PuissanceDisponibleDuPalierThermiqueRef_SV    ; 
	
	RESULTATS_HORAIRES *                  ResultatsHoraires;
	PALIERS_THERMIQUES *                  PaliersThermiquesDuPays;
  CLASSE_DE_MANOEUVRABILITE *           ClasseDeManoeuvrabiliteArray;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER ** PuissanceDisponibleEtCout;
  PRODUCTION_THERMIQUE_OPTIMALE **      ProductionThermiqueOptimale;
	
	LongueurDuPasDeTemps = (int) ClasseDeManoeuvrabilite;
	MaxPminJour = ProblemeHebdo->maxPminThermiqueByDay;

  NombreDePasDeTempsProblemeHebdo = ProblemeHebdo->NombreDePasDeTemps;
	NumeroDeJourDuPasDeTemps        = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		ResultatsHoraires            = ProblemeHebdo->ResultatsHoraires[Pays];
		PaliersThermiquesDuPays      = ProblemeHebdo->PaliersThermiquesDuPays[Pays];		
		ClasseDeManoeuvrabiliteArray = PaliersThermiquesDuPays->ClasseDeManoeuvrabilite;		
		PuissanceDisponibleEtCout    = PaliersThermiquesDuPays->PuissanceDisponibleEtCout;
		
    PminDuPalierThermiquePendantUneHeure     = PaliersThermiquesDuPays->PminDuPalierThermiquePendantUneHeure;		
		TailleUnitaireDUnGroupeDuPalierThermique = PaliersThermiquesDuPays->TailleUnitaireDUnGroupeDuPalierThermique;

		ProductionThermiqueOptimale = ResultatsHoraires->ProductionThermique;
		
		for ( Index = 0; Index < PaliersThermiquesDuPays->NombreDePaliersThermiques; Index++ ) {
			if ( ClasseDeManoeuvrabiliteArray[Index] != ClasseDeManoeuvrabilite ) continue;
			
			PuissanceDisponibleDuPalierThermiqueRef    = PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermiqueRef; 			
			PuissanceDisponibleDuPalierThermiqueRef_SV = PuissanceDisponibleEtCout[Index]->PuissanceDisponibleDuPalierThermiqueRef_SV;  
			
			

      
      OPT_CalculMaxPminJour( ProblemeHebdo, PaliersThermiquesDuPays, ProductionThermiqueOptimale,
			                       TailleUnitaireDUnGroupeDuPalierThermique, Index, MaxPminJour );						
		
						
			
			for ( PdtHebdo = 0, PdtHeure = 0; PdtHebdo < NombreDePasDeTempsProblemeHebdo ; PdtHebdo++ ) {
				P = ProductionThermiqueOptimale[PdtHebdo]->ProductionThermiqueDuPalier[Index];
				
				if ( TailleUnitaireDUnGroupeDuPalierThermique[Index] > 0 ) {
				  NbGroupesEquivalents = P / TailleUnitaireDUnGroupeDuPalierThermique[Index];
				}
				else {
					NbGroupesEquivalents = P;
				}
				
				NbGroupesEquivalents = ceil( NbGroupesEquivalents );
				Pmin = PminDuPalierThermiquePendantUneHeure[Index] * NbGroupesEquivalents;
				
				Jour = NumeroDeJourDuPasDeTemps[PdtHebdo];
        if ( Pmin < MaxPminJour[Jour] ) Pmin = MaxPminJour[Jour];				
				
        if ( Pmin > P ) P = Pmin;				
				

				for ( i = 0 ; i < LongueurDuPasDeTemps ; i++ ) {				 
					PuissanceDisponibleDuPalierThermiqueRef[PdtHeure] = P;
					

					if ( P < ZERO_PMAX ) {
						if ( PuissanceDisponibleDuPalierThermiqueRef_SV[PdtHeure] >= ZERO_PMAX ) {
					    PuissanceDisponibleDuPalierThermiqueRef[PdtHeure] = ZERO_PMAX;
						}
					}
					
					if ( PuissanceDisponibleDuPalierThermiqueRef[PdtHeure] > PuissanceDisponibleDuPalierThermiqueRef_SV[PdtHeure] ) {
					  PuissanceDisponibleDuPalierThermiqueRef[PdtHeure] = PuissanceDisponibleDuPalierThermiqueRef_SV[PdtHeure];
					}					
					PdtHeure++;
				}
			}						
		}
	}
		 
	return;
}




void OPT_SauvegarderLesPmaxThermiques( PROBLEME_HEBDO * ProblemeHebdo )  
{
	int Pays; int Palier; double * PuissanceDisponibleDuPalierThermiqueRef;
	double * PuissanceDisponibleDuPalierThermiqueRef_SV;
	
	PALIERS_THERMIQUES * PaliersThermiquesDuPays;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER * PuissanceDisponibleEtCout; 

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		for ( Palier = 0; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques; Palier++ ) {
			PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
			PuissanceDisponibleDuPalierThermiqueRef    = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef;
			PuissanceDisponibleDuPalierThermiqueRef_SV = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef_SV;
			
			memcpy( (char *) PuissanceDisponibleDuPalierThermiqueRef_SV, (char *) PuissanceDisponibleDuPalierThermiqueRef,
							ProblemeHebdo->NombreDePasDeTempsRef * sizeof( double ) );
			
		}		
	}
	return;
}




void OPT_RestaurerLesPmaxThermiques( PROBLEME_HEBDO * ProblemeHebdo )  
{
	int Pays; int Palier; double * PuissanceDisponibleDuPalierThermiqueRef;
	double * PuissanceDisponibleDuPalierThermiqueRef_SV;
	
	PALIERS_THERMIQUES * PaliersThermiquesDuPays;
  PDISP_ET_COUTS_HORAIRES_PAR_PALIER * PuissanceDisponibleEtCout; 

	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
		for ( Palier = 0; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques; Palier++ ) {
			PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
			PuissanceDisponibleDuPalierThermiqueRef    = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef;
			PuissanceDisponibleDuPalierThermiqueRef_SV = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef_SV;
			
			memcpy( (char *) PuissanceDisponibleDuPalierThermiqueRef, (char *) PuissanceDisponibleDuPalierThermiqueRef_SV,
							ProblemeHebdo->NombreDePasDeTempsRef * sizeof( double ) );
			
		}
	}
	return;
}

