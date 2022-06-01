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

#include "opt_structure_probleme_a_resoudre.h"

#include "../simulation/simulation.h"
#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include <iostream>

void OPT_RestaurerLesDonnees(PROBLEME_HEBDO* ProblemeHebdo)
{
    int Pays;
    int Interco;
    int Pdt;
    int DernierPasDeTemps;
    int Jour;
    int Palier;
    int CntCouplante;
    double PmaxHydEcretee;
    int Intervalle;
    int Semaine;
    double PmaxHydUplift; // Hydro generating power required to make use of energy stored from pumps

    CONSOMMATIONS_ABATTUES* ConsommationsAbattues;
    CONSOMMATIONS_ABATTUES* ConsommationsAbattuesRef;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC;
    VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTCRef;

    RESERVE_JMOINS1* ReserveJMoins1;
    ENERGIES_ET_PUISSANCES_HYDRAULIQUES* CaracteristiquesHydrauliques;
    PDISP_ET_COUTS_HORAIRES_PAR_PALIER* PuissanceDisponibleEtCout;
    PALIERS_THERMIQUES* PaliersThermiquesDuPays;
    CONTRAINTES_COUPLANTES* MatriceDesContraintesCouplantes;
    COUTS_DE_TRANSPORT* CoutDeTransport;
    int* NumeroDeJourDuPasDeTemps;

    DernierPasDeTemps = ProblemeHebdo->NombreDePasDeTemps;
    NumeroDeJourDuPasDeTemps = ProblemeHebdo->NumeroDeJourDuPasDeTemps;

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        ValeursDeNTCRef = ProblemeHebdo->ValeursDeNTCRef[Pdt];
        ValeursDeNTC = ProblemeHebdo->ValeursDeNTC[Pdt];

        for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
        {
            ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco]
              = ValeursDeNTCRef->ValeurDeNTCOrigineVersExtremite[Interco];
            ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]
              = ValeursDeNTCRef->ValeurDeNTCExtremiteVersOrigine[Interco];
            ValeursDeNTC->ValeurDeLoopFlowOrigineVersExtremite[Interco]
              = ValeursDeNTCRef->ValeurDeLoopFlowOrigineVersExtremite[Interco];
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        for (Interco = 0; Interco < ProblemeHebdo->NombreDInterconnexions; Interco++)
        {
            CoutDeTransport = ProblemeHebdo->CoutDeTransport[Interco];
            if (CoutDeTransport->IntercoGereeAvecDesCouts == OUI_ANTARES)
            {
                CoutDeTransport->CoutDeTransportOrigineVersExtremite[Pdt]
                  = CoutDeTransport->CoutDeTransportOrigineVersExtremiteRef[Pdt];
                CoutDeTransport->CoutDeTransportExtremiteVersOrigine[Pdt]
                  = CoutDeTransport->CoutDeTransportExtremiteVersOrigineRef[Pdt];
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        ConsommationsAbattuesRef = ProblemeHebdo->ConsommationsAbattuesRef[Pdt];
        ConsommationsAbattues = ProblemeHebdo->ConsommationsAbattues[Pdt];
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            ConsommationsAbattues->ConsommationAbattueDuPays[Pays]
              = ConsommationsAbattuesRef->ConsommationAbattueDuPays[Pays];
        }
    }

    if (ProblemeHebdo->YaDeLaReserveJmoins1 == OUI_ANTARES
        && ProblemeHebdo->ProblemeAResoudre->NumeroDOptimisation == PREMIERE_OPTIMISATION)
    {
        for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
        {
            for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
            {
                ReserveJMoins1 = ProblemeHebdo->ReserveJMoins1[Pays];
                ReserveJMoins1->ReserveHoraireJMoins1[Pdt]
                  = ReserveJMoins1->ReserveHoraireJMoins1Ref[Pdt];
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];

            if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable != OUI_ANTARES)
                continue;

            CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt]
              = CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraireRef[Pdt];
            if (CaracteristiquesHydrauliques->SansHeuristique == OUI_ANTARES)
                continue;
            Jour = NumeroDeJourDuPasDeTemps[Pdt];
            PmaxHydEcretee = CaracteristiquesHydrauliques->CntEnergieH2OParJour[Jour];
            PmaxHydEcretee *= ProblemeHebdo->CoefficientEcretementPMaxHydraulique[Pays];
            PmaxHydEcretee /= (double)ProblemeHebdo->NombreDePasDeTempsDUneJournee;
            // The primary generating power allowance may need to be uplifted to match pumping power
            // allowance
            if (CaracteristiquesHydrauliques->PresenceDePompageModulable == OUI_ANTARES)
            {
                PmaxHydUplift = CaracteristiquesHydrauliques->ContrainteDePmaxPompageHoraire[Pdt];
                PmaxHydUplift *= ProblemeHebdo->CoefficientEcretementPMaxHydraulique[Pays];

                if (PmaxHydEcretee < PmaxHydUplift)
                    PmaxHydEcretee = PmaxHydUplift;
            }

            // The generating power allowance cannot exceed the maximum available generating power
            if (PmaxHydEcretee
                < CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt])
            {
                CaracteristiquesHydrauliques->ContrainteDePmaxHydrauliqueHoraire[Pdt]
                  = PmaxHydEcretee;
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps;)
    {
        Intervalle = ProblemeHebdo->NumeroDIntervalleOptimiseDuPasDeTemps[Pdt];
        Pdt += ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];
            if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable == OUI_ANTARES)
            {
                CaracteristiquesHydrauliques->CntEnergieH2OParIntervalleOptimise[Intervalle]
                  = CaracteristiquesHydrauliques->CntEnergieH2OParIntervalleOptimiseRef[Intervalle];
            }
        }
    }

    for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
    {
        CaracteristiquesHydrauliques = ProblemeHebdo->CaracteristiquesHydrauliques[Pays];
        if (CaracteristiquesHydrauliques->PresenceDHydrauliqueModulable == OUI_ANTARES)
        {
            CaracteristiquesHydrauliques->MaxDesPmaxHydrauliques
              = CaracteristiquesHydrauliques->MaxDesPmaxHydrauliquesRef;
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Palier = 0; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques; Palier++)
            {
                PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
                PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[Pdt]
                  = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique_SV[Pdt];
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        for (Pays = 0; Pays < ProblemeHebdo->NombreDePays; Pays++)
        {
            PaliersThermiquesDuPays = ProblemeHebdo->PaliersThermiquesDuPays[Pays];
            for (Palier = 0; Palier < PaliersThermiquesDuPays->NombreDePaliersThermiques; Palier++)
            {
                PuissanceDisponibleEtCout
                  = PaliersThermiquesDuPays->PuissanceDisponibleEtCout[Palier];
                PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[Pdt]
                  = PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermiqueRef[Pdt];

                if (PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[Pdt]
                    > PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[Pdt])
                {
                    PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[Pdt]
                      = PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[Pdt];

                    if (PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[Pdt]
                        > PuissanceDisponibleEtCout
                            ->PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt])
                    {
                        PuissanceDisponibleEtCout->PuissanceDisponibleDuPalierThermique[Pdt]
                          = PuissanceDisponibleEtCout
                              ->PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt];
                        PuissanceDisponibleEtCout->PuissanceMinDuPalierThermique[Pdt]
                          = PuissanceDisponibleEtCout
                              ->PuissanceDisponibleDuPalierThermiqueRef_SV[Pdt];
                    }
                }

                PuissanceDisponibleEtCout->CoutHoraireDeProductionDuPalierThermique[Pdt]
                  = PuissanceDisponibleEtCout->CoutHoraireDeProductionDuPalierThermiqueRef[Pdt];
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps; Pdt++)
    {
        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];

            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante == CONTRAINTE_HORAIRE)
            {
                MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Pdt]
                  = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplanteRef[Pdt];
            }
        }
    }

    for (Pdt = 0; Pdt < DernierPasDeTemps;)
    {
        Jour = ProblemeHebdo->NumeroDeJourDuPasDeTemps[Pdt];
        Pdt += ProblemeHebdo->NombreDePasDeTempsDUneJournee;
        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_JOURNALIERE)
            {
                MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Jour]
                  = MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplanteRef[Jour];
            }
        }
    }

    if (ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > ProblemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        Semaine = 0;
        for (CntCouplante = 0; CntCouplante < ProblemeHebdo->NombreDeContraintesCouplantes;
             CntCouplante++)
        {
            MatriceDesContraintesCouplantes
              = ProblemeHebdo->MatriceDesContraintesCouplantes[CntCouplante];
            if (MatriceDesContraintesCouplantes->TypeDeContrainteCouplante
                == CONTRAINTE_HEBDOMADAIRE)
            {
                MatriceDesContraintesCouplantes->SecondMembreDeLaContrainteCouplante[Semaine]
                  = MatriceDesContraintesCouplantes
                      ->SecondMembreDeLaContrainteCouplanteRef[Semaine];
            }
        }
    }

    return;
}
