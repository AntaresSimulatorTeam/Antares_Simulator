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





void OPT_GenererLesDonneesSelonLePasDeTempsDeLaClasseDeManoeuvrabilite(
    PROBLEME_HEBDO * ProblemeHebdo, const CLASSE_DE_MANOEUVRABILITE ClasseDeManoeuvrabilite )
{
	int Pays; int Interco; int Pdt; int PdtHoraire; int DernierPasDeTemps;
	int i; double SommeReserve; int PdtHoraireDebut; double SommePmaxHydrau; int Jour; double SommePmax;
	int Palier; double SommeCouts; double SommeCntCouplante; int CntCouplante; double PmaxHydEcretee;
  int Intervalle; int Semaine;
	double SommeCoutOversE; double SommeCoutEversO; double SommePmin;
	CONSOMMATIONS_ABATTUES              * ConsommationsAbattues;
	CONSOMMATIONS_ABATTUES              * ConsommationsAbattuesRef;
	VALEURS_DE_NTC_ET_RESISTANCES       * ValeursDeNTC;
	VALEURS_DE_NTC_ET_RESISTANCES       * ValeursDeNTCRef;
	
	RESERVE_JMOINS1                     * ReserveJMoins1;
	ENERGIES_ET_PUISSANCES_HYDRAULIQUES * CaracteristiquesHydrauliques;
	PDISP_ET_COUTS_HORAIRES_PAR_PALIER  * PuissanceDisponibleEtCout;
	PALIERS_THERMIQUES                  * PaliersThermiquesDuPays;
	CONTRAINTES_COUPLANTES              * MatriceDesContraintesCouplantes;
	COUTS_DE_TRANSPORT                  * CoutDeTransport;
	double * ValeurDeNTCOrigineVersExtremite; double * ValeurDeNTCExtremiteVersOrigine;
  double * CoutDeTransportOrigineVersExtremiteRef; double * CoutDeTransportExtremiteVersOrigineRef;
  double * CoutDeTransportOrigineVersExtremite;	double * CoutDeTransportExtremiteVersOrigine;
  double * PuissanceDisponibleDuPalierThermiqueRef; double * CoutHoraireDeProductionDuPalierThermiqueRef;
  double * PuissanceDisponibleDuPalierThermique; double * CoutHoraireDeProductionDuPalierThermique;
  double * PuissanceMinDuPalierThermique_SV; double * PuissanceMinDuPalierThermique;
  double * PuissanceDisponibleDuPalierThermiqueRef_SV; 	
	double * SommeOversE;
	double * SommeEversO;
	double * Somme;
	int * NumeroDeJourDuPasDeTemps;
	
	
  DernierPasDeTemps = ProblemeHebdo->NombreDePasDeTemps;		
	NumeroDeJourDuPasDeTemps = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

	if (ClasseDeManoeuvrabilite == 1) {
    OPT_RestaurerLesDonneesSelonManoeuvrabilite_1( ProblemeHebdo );		
		return;
  }

	SommeOversE = ProblemeHebdo->manoeuvrabilite.sumOversE;
	SommeEversO = ProblemeHebdo->manoeuvrabilite.sumEversO;
	Somme       = ProblemeHebdo->manoeuvrabilite.sum;
	
	
	PdtHoraire = 0;
	
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		
		for ( Interco = 0 ; Interco < ProblemeHebdo->NombreDInterconnexions ; Interco++ ) {
			SommeOversE[Interco] = 0.;
			SommeEversO[Interco] = 0.;
		}

		for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
			ValeursDeNTCRef = ProblemeHebdo->ValeursDeNTCRef[PdtHoraire];
			ValeurDeNTCOrigineVersExtremite = ValeursDeNTCRef->ValeurDeNTCOrigineVersExtremite;
			ValeurDeNTCExtremiteVersOrigine = ValeursDeNTCRef->ValeurDeNTCExtremiteVersOrigine;
			for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++) {
				SommeOversE[Interco]+=  ValeurDeNTCOrigineVersExtremite[Interco];
				SommeEversO[Interco]+=  ValeurDeNTCExtremiteVersOrigine[Interco];
			}
			PdtHoraire++;
		}

		ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[Pdt];
		ValeurDeNTCOrigineVersExtremite = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite;
		ValeurDeNTCExtremiteVersOrigine = ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine;
		for ( Interco = 0 ; Interco < ProblemeHebdo->NombreDInterconnexions ; Interco++ ) {
			ValeurDeNTCOrigineVersExtremite[Interco] = SommeOversE[Interco] / ClasseDeManoeuvrabilite;
			ValeurDeNTCExtremiteVersOrigine[Interco] = SommeEversO[Interco] / ClasseDeManoeuvrabilite;
		}
	}
	
	if ( COUT_TRANSPORT == OUI_ANTARES ) {
		
		PdtHoraireDebut = 0;
		for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
			
			for ( Interco = 0 ; Interco < ProblemeHebdo->NombreDInterconnexions ; Interco++ ) {
				CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
        CoutDeTransportOrigineVersExtremiteRef = CoutDeTransport->CoutDeTransportOrigineVersExtremiteRef;
				CoutDeTransportExtremiteVersOrigineRef = CoutDeTransport->CoutDeTransportExtremiteVersOrigineRef;				
        CoutDeTransportOrigineVersExtremite = CoutDeTransport->CoutDeTransportOrigineVersExtremite;
        CoutDeTransportExtremiteVersOrigine = CoutDeTransport->CoutDeTransportExtremiteVersOrigine;				
				if ( CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES ) {
					SommeCoutOversE = 0.0;
					SommeCoutEversO = 0.0;
					PdtHoraire      = PdtHoraireDebut;
					for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++) {
						SommeCoutOversE+= CoutDeTransportOrigineVersExtremiteRef[PdtHoraire];
						SommeCoutEversO+= CoutDeTransportExtremiteVersOrigineRef[PdtHoraire];
						PdtHoraire++;
					}
					CoutDeTransportOrigineVersExtremite[Pdt] = SommeCoutOversE / ClasseDeManoeuvrabilite; 		
					CoutDeTransportExtremiteVersOrigine[Pdt] = SommeCoutEversO / ClasseDeManoeuvrabilite; 		
				}
			}
			PdtHoraireDebut+= ClasseDeManoeuvrabilite;
		}
	}

	
	
	PdtHoraire = 0;
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		  
		memset( Somme, '\0', sizeof(double) * ProblemeHebdo->NombreDePays );

		for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
			ConsommationsAbattuesRef = ProblemeHebdo->ConsommationsAbattuesRef[PdtHoraire];
			for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
				
				Somme[Pays] += ConsommationsAbattuesRef->ConsommationAbattueDuPays[Pays];
			}
			PdtHoraire++;
		}
		ConsommationsAbattues = ProblemeHebdo->ConsommationsAbattues[Pdt];
		for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; ++Pays)
			ConsommationsAbattues->ConsommationAbattueDuPays[Pays] = Somme[Pays] / ClasseDeManoeuvrabilite;
	}

	




















	if ( ProblemeHebdo->YaDeLaReserveJmoins1 == OUI_ANTARES && (ProblemeHebdo->ProblemeAResoudre)->NumeroDOptimisation == PREMIERE_OPTIMISATION ) {
		
		PdtHoraireDebut = 0;
		for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
			
			for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++) {
				ReserveJMoins1 = ProblemeHebdo->ReserveJMoins1[Pays];
				PdtHoraire     = PdtHoraireDebut;
				SommeReserve   = 0.0;
				for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
					SommeReserve+= ReserveJMoins1->ReserveHoraireJMoins1Ref[PdtHoraire];
					PdtHoraire++;
				}
				ReserveJMoins1->ReserveHoraireJMoins1[Pdt] = SommeReserve / ClasseDeManoeuvrabilite;
			}
			PdtHoraireDebut = PdtHoraire;
		}
	}

	
	
	PdtHoraireDebut = 0;	
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
			CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];
			
			if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable != OUI_ANTARES) continue;

			PdtHoraire      = PdtHoraireDebut;
			SommePmaxHydrau = 0.0;
			for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
				SommePmaxHydrau += CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraireRef[PdtHoraire];
				PdtHoraire++;
			}   
			
			CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt] = SommePmaxHydrau / ClasseDeManoeuvrabilite;

			

			

			





			Jour = NumeroDeJourDuPasDeTemps[Pdt];
			PmaxHydEcretee = CaracteristiquesHydrauliques->CntEnergieH2OParJour[Jour]; 		
			PmaxHydEcretee *= ProblemeHebdo->CoefficientEcretementPMaxHydraulique[Pays];  			
			PmaxHydEcretee /= (double) ProblemeHebdo->NombreDePasDeTempsDUneJournee;
			
			if ( PmaxHydEcretee < CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt] ) {
				CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt] = PmaxHydEcretee;
			}
		}
		PdtHoraireDebut = PdtHoraire;
	}

	
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; ) {
		Intervalle = ProblemeHebdo->NumeroDIntervalleOptimiseDuPasDeTemps[Pdt];
		Pdt+= ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
		for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
			CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];
			if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable == OUI_ANTARES) {
				CaracteristiquesHydrauliques->CntEnergieH2OParIntervalleOptimise[Intervalle] =
					CaracteristiquesHydrauliques->CntEnergieH2OParIntervalleOptimiseRef[Intervalle] / ClasseDeManoeuvrabilite;
			}
		}
	}
	for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
		CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];
		if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable == OUI_ANTARES) {
			CaracteristiquesHydrauliques->MaxDesPmaxHydrauliques = CaracteristiquesHydrauliques->MaxDesPmaxHydrauliquesRef / ClasseDeManoeuvrabilite;
		}
	}
	
	
	PdtHoraireDebut = 0;
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		for (Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
			PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
			for ( Palier = 0 ; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Palier++ ) {
				PuissanceDisponibleEtCout        = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
        PuissanceMinDuPalierThermique_SV = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique_SV;
        PuissanceMinDuPalierThermique    = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique;				
				PdtHoraire = PdtHoraireDebut;
				SommePmin  = 0.0;
				for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
					SommePmin+=  PuissanceMinDuPalierThermique_SV[PdtHoraire];
					PdtHoraire++;
				}
				PuissanceMinDuPalierThermique[Pdt] = SommePmin / ClasseDeManoeuvrabilite;
			}
		}
		PdtHoraireDebut = PdtHoraire;
	}
		
	
	PdtHoraireDebut = 0;
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		for ( Pays = 0 ; Pays < ProblemeHebdo->NombreDePays ; Pays++ ) {
			PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
			for ( Palier = 0 ; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques ; Palier++ ) {
				PuissanceDisponibleEtCout = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
        PuissanceDisponibleDuPalierThermiqueRef     = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef;
        CoutHoraireDeProductionDuPalierThermiqueRef = PuissanceDisponibleEtCout->CoutHoraireDeProductionDuPalierThermiqueRef;
				PuissanceDisponibleDuPalierThermique        = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique;
        PuissanceDisponibleDuPalierThermiqueRef_SV  = PuissanceDisponibleEtCout-> PuissanceDisponibleDuPalierThermiqueRef_SV;				
				CoutHoraireDeProductionDuPalierThermique    = PuissanceDisponibleEtCout->CoutHoraireDeProductionDuPalierThermique;
				PuissanceMinDuPalierThermique               = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique;				
				PdtHoraire = PdtHoraireDebut;
				SommePmax  = 0.0;
				SommeCouts = 0.0;
				for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
					SommePmax+=  PuissanceDisponibleDuPalierThermiqueRef[PdtHoraire];
					SommeCouts+= CoutHoraireDeProductionDuPalierThermiqueRef[PdtHoraire];
					PdtHoraire++;
				}
				PuissanceDisponibleDuPalierThermique[Pdt] = SommePmax / ClasseDeManoeuvrabilite;				
				
												
				if ( PuissanceMinDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermique[Pdt]) {
				  PuissanceDisponibleDuPalierThermique[Pdt] = PuissanceMinDuPalierThermique[Pdt];
					if ( PuissanceDisponibleDuPalierThermique[Pdt] > PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt] ) {
					  PuissanceDisponibleDuPalierThermique[Pdt] = PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt];
						PuissanceMinDuPalierThermique       [Pdt] = PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt];
					}					
				}				
				CoutHoraireDeProductionDuPalierThermique[Pdt] = SommeCouts/ ClasseDeManoeuvrabilite; 
			}
		}
		PdtHoraireDebut = PdtHoraire;
	}
	
	
	PdtHoraireDebut = 0;
	for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; Pdt++ ) {
		
		for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
			MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
			if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE ) {
			SommeCntCouplante = 0.0;
			PdtHoraire        = PdtHoraireDebut;
			for ( i = 0 ; i < ClasseDeManoeuvrabilite ; i++ ) {
				SommeCntCouplante+= MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplanteRef[PdtHoraire];
				PdtHoraire++;
			}
			MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Pdt] = SommeCntCouplante / ClasseDeManoeuvrabilite;			
		}
		}
		PdtHoraireDebut+= ClasseDeManoeuvrabilite;
	}

	
  for ( Pdt = 0 ; Pdt < DernierPasDeTemps ; ) {
	  Jour = ProblemeHebdo->NumeroDeJourDuPasDeTemps[Pdt];
	  Pdt+= ProblemeHebdo->NombreDePasDeTempsDUneJournee;
	  for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
	 	  MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
		  if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_JOURNALIERE ) {
		  MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Jour] =
        MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplanteRef[Jour] / ClasseDeManoeuvrabilite;
		}
	  }
	}
	
	
	if ( ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation > ProblemeHebdo->NombreDePasDeTempsDUneJournee ) {
	  Semaine = 0;
	  for ( CntCouplante = 0 ; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes ; CntCouplante++ ) {
	 	  MatriceDesContraintesCouplantes = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
		  if ( MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HEBDOMADAIRE ) {
		  MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Semaine] =
        MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplanteRef[Semaine] / ClasseDeManoeuvrabilite;
	  }
	  }
	}
	
	return;
	
}



