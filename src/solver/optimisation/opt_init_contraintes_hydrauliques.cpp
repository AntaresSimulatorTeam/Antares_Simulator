/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/simulation.h"

void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(
  PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsPourUneOptimisation = problemeHebdo
                                                  ->NombreDePasDeTempsPourUneOptimisation;
    int NbIntervallesOptimises = problemeHebdo->NombreDePasDeTemps
                                 / NombreDePasDeTempsPourUneOptimisation;
    int NombreDeJoursParIntervalle = NombreDePasDeTempsPourUneOptimisation
                                     / problemeHebdo->NombreDePasDeTempsDUneJournee;

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable
            || problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes)
        {
            continue;
        }

        std::vector<double>& CntEnergieH2OParJour = problemeHebdo
                                                      ->CaracteristiquesHydrauliques[pays]
                                                      .CntEnergieH2OParJour;
        std::vector<double>& CntEnergieH2OParIntervalleOptimise
          = problemeHebdo->CaracteristiquesHydrauliques[pays].CntEnergieH2OParIntervalleOptimise;

        for (int intervalle = 0; intervalle < NbIntervallesOptimises; intervalle++)
        {
            double CntTurbParIntervalle = 0.0;
            for (int jour = 0; jour < NombreDeJoursParIntervalle; jour++)
            {
                int index = intervalle * NombreDeJoursParIntervalle + jour;
                CntTurbParIntervalle += CntEnergieH2OParIntervalleOptimise[index];

                CntEnergieH2OParJour[index] = CntEnergieH2OParIntervalleOptimise[index];
            }

            CntEnergieH2OParIntervalleOptimise[intervalle] = CntTurbParIntervalle;
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro = problemeHebdo->CaracteristiquesHydrauliques[pays]
                               .PresenceDHydrauliqueModulable;
        bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                 .TurbinageEntreBornes;

        if (presenceHydro
            && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
        {
            std::vector<double>& CntEnergieH2OParJour = problemeHebdo
                                                          ->CaracteristiquesHydrauliques[pays]
                                                          .CntEnergieH2OParJour;
            std::vector<double>& MinEnergieHydrauParIntervalleOptimise
              = problemeHebdo->CaracteristiquesHydrauliques[pays]
                  .MinEnergieHydrauParIntervalleOptimise;
            std::vector<double>& MaxEnergieHydrauParIntervalleOptimise
              = problemeHebdo->CaracteristiquesHydrauliques[pays]
                  .MaxEnergieHydrauParIntervalleOptimise;

            for (int intervalle = 0; intervalle < NbIntervallesOptimises; intervalle++)
            {
                double CntMinEParIntervalle = 0.;
                double CntMaxEParIntervalle = 0.;
                for (int jour = 0; jour < NombreDeJoursParIntervalle; jour++)
                {
                    int index = intervalle * NombreDeJoursParIntervalle + jour;
                    CntMinEParIntervalle += MinEnergieHydrauParIntervalleOptimise[index];
                    CntMaxEParIntervalle += MaxEnergieHydrauParIntervalleOptimise[index];

                    CntEnergieH2OParJour[index] = MaxEnergieHydrauParIntervalleOptimise[index];
                }
                MinEnergieHydrauParIntervalleOptimise[intervalle] = CntMinEParIntervalle;
                MaxEnergieHydrauParIntervalleOptimise[intervalle] = CntMaxEParIntervalle;
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        std::vector<double>& MaxEnergiePompageParIntervalleOptimise
          = problemeHebdo->CaracteristiquesHydrauliques[pays]
              .MaxEnergiePompageParIntervalleOptimise;

        for (int intervalle = 0; intervalle < NbIntervallesOptimises; intervalle++)
        {
            double MaxPompageParIntervalle = 0.;
            for (int jour = 0; jour < NombreDeJoursParIntervalle; jour++)
            {
                int index = intervalle * NombreDeJoursParIntervalle + jour;
                MaxPompageParIntervalle += MaxEnergiePompageParIntervalleOptimise[index];
            }

            MaxEnergiePompageParIntervalleOptimise[intervalle] = MaxPompageParIntervalle;
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        std::vector<double>& InflowForTimeInterval = problemeHebdo
                                                       ->CaracteristiquesHydrauliques[pays]
                                                       .InflowForTimeInterval;

        for (int intervalle = 0; intervalle < NbIntervallesOptimises; intervalle++)
        {
            double InflowSum = 0.;
            for (int jour = 0; jour < NombreDeJoursParIntervalle; jour++)
            {
                int index = intervalle * NombreDeJoursParIntervalle + jour;
                InflowSum += InflowForTimeInterval[index];
            }

            InflowForTimeInterval[intervalle] = InflowSum;
        }
    }

    return;
}
