/*
** Copyright 2007-2023 RTE
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

#include "spx_constantes_externes.h"
#include "../simulation/sim_structure_probleme_adequation.h"

void OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(
  PROBLEME_HEBDO* problemeHebdo)
{
    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int NbIntervallesOptimises
      = problemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation;
    int NombreDeJoursParIntervalle
      = NombreDePasDeTempsPourUneOptimisation / problemeHebdo->NombreDePasDeTempsDUneJournee;

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (!problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable
            || problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes)
            continue;

        std::vector<double>& CntEnergieH2OParJour
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->CntEnergieH2OParJour;
        std::vector<double>& CntEnergieH2OParIntervalleOptimise
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->CntEnergieH2OParIntervalleOptimise;
        std::vector<double>& CntEnergieH2OParIntervalleOptimiseRef
          = problemeHebdo->CaracteristiquesHydrauliques[pays]
              ->CntEnergieH2OParIntervalleOptimiseRef;

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
            CntEnergieH2OParIntervalleOptimiseRef[intervalle] = CntTurbParIntervalle;
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        bool presenceHydro
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDHydrauliqueModulable;
        bool TurbEntreBornes
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->TurbinageEntreBornes;

        if (presenceHydro
            && (TurbEntreBornes
                || problemeHebdo->CaracteristiquesHydrauliques[pays]->PresenceDePompageModulable))
        {
            std::vector<double>& CntEnergieH2OParJour
              = problemeHebdo->CaracteristiquesHydrauliques[pays]->CntEnergieH2OParJour;
            std::vector<double>& MinEnergieHydrauParIntervalleOptimise
              = problemeHebdo->CaracteristiquesHydrauliques[pays]
                  ->MinEnergieHydrauParIntervalleOptimise;
            std::vector<double>& MaxEnergieHydrauParIntervalleOptimise
              = problemeHebdo->CaracteristiquesHydrauliques[pays]
                  ->MaxEnergieHydrauParIntervalleOptimise;

            for (int intervalle = 0; intervalle < NbIntervallesOptimises; intervalle++)
            {
                double CntMinEParIntervalle = 0.;
                double CntMaxEParIntervalle = 0.;
                for (int jour = 0; jour < NombreDeJoursParIntervalle; jour++)
                {
                    int index = intervalle * NombreDeJoursParIntervalle + jour;
                    CntMinEParIntervalle += MinEnergieHydrauParIntervalleOptimise[index];
                    CntMaxEParIntervalle += MaxEnergieHydrauParIntervalleOptimise[index];

                    CntEnergieH2OParJour[index] = problemeHebdo->CaracteristiquesHydrauliques[pays]
                                                    ->MaxEnergieHydrauParIntervalleOptimise[index];
                }
                MinEnergieHydrauParIntervalleOptimise[intervalle] = CntMinEParIntervalle;
                MaxEnergieHydrauParIntervalleOptimise[intervalle] = CntMaxEParIntervalle;
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        std::vector<double>& MaxEnergiePompageParIntervalleOptimise
          = problemeHebdo->CaracteristiquesHydrauliques[pays]
              ->MaxEnergiePompageParIntervalleOptimise;

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

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        std::vector<double>& InflowForTimeInterval
          = problemeHebdo->CaracteristiquesHydrauliques[pays]->InflowForTimeInterval;

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
