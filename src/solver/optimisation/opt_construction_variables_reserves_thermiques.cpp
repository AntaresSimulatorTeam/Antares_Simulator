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

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "antares/solver/simulation/sim_extern_variables_globales.h"

#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_rename_problem.h"

#include <spx_constantes_externes.h>

void OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaireReservesThermiques(
  PROBLEME_HEBDO* problemeHebdo,
  bool Simulation)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    int& NombreDeVariables = ProblemeAResoudre->NombreDeVariables;
    VariableNamer variableNamer(ProblemeAResoudre->NomDesVariables);
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        variableNamer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        auto& CorrespondanceVarNativesVarOptim
          = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            variableNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            auto areaReserves = problemeHebdo->allReserves.thermalAreaReserves[pays];
            int index = 0;
            int reserveIndex = 0;
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            for (auto& areaReserveUp : areaReserves.areaCapacityReservationsUp)
            {
                reserveIndex = areaReserveUp.globalReserveIndex;
                if (Simulation)
                {
                    NombreDeVariables += 2;
                }
                else
                {
                    // For Unsatisfied Reserves
                    variableManager.InternalUnsatisfiedReserve(reserveIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalUnsatisfiedReserve(NombreDeVariables,
                                                             areaReserveUp.reserveName);
                    NombreDeVariables++;

                    // For Excess Reserves
                    variableManager.InternalExcessReserve(reserveIndex, pdt) = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalExcessReserve(NombreDeVariables,
                                                        areaReserveUp.reserveName);
                    NombreDeVariables++;
                }

                int clusterIndex = 0;
                for (auto& clusterReserveParticipation :
                     areaReserveUp.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        const auto& clusterName
                          = PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex];
                        if (Simulation)
                        {
                            NombreDeVariables += 2;
                        }
                        else
                        {
                            // For running units in cluster
                            variableManager.ClusterReserveParticipation(clusterReserveParticipation.globalIndexClusterParticipation, pdt)
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfRunningUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveUp.reserveName);
                            NombreDeVariables++;

                            // For all units in cluster (off units can participate to the reserves)
                            variableManager.RunningClusterReserveParticipation(clusterReserveParticipation.globalIndexClusterParticipation, pdt)
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveUp.reserveName);
                            NombreDeVariables++;

                            clusterIndex++;
                        }
                    }
                }
            }
            for (auto& areaReserveDown : areaReserves.areaCapacityReservationsDown)
            {
                reserveIndex = areaReserveDown.globalReserveIndex;
                if (Simulation)
                {
                    NombreDeVariables += 2;
                }
                else
                {
                    // For Unsatisfied Reserves
                    variableManager.InternalUnsatisfiedReserve(reserveIndex, pdt)
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalUnsatisfiedReserve(NombreDeVariables,
                                                             areaReserveDown.reserveName);
                    NombreDeVariables++;

                    // For Excess Reserves
                    variableManager.InternalExcessReserve(reserveIndex, pdt) = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalExcessReserve(NombreDeVariables,
                                                        areaReserveDown.reserveName);
                    NombreDeVariables++;
                }

                int clusterIndex = 0;
                for (auto& clusterReserveParticipation :
                     areaReserveDown.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        const auto& clusterName
                          = PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex];
                        if (Simulation)
                        {
                            NombreDeVariables += 2;
                        }
                        else
                        {
                            // For running units in cluster
                            variableManager.ClusterReserveParticipation(clusterReserveParticipation.globalIndexClusterParticipation, pdt)
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfRunningUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveDown.reserveName);
                            NombreDeVariables++;

                            // For all units in cluster (off units can participate to the reserves)
                            variableManager.RunningClusterReserveParticipation(clusterReserveParticipation.globalIndexClusterParticipation, pdt)
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveDown.reserveName);
                            NombreDeVariables++;

                            clusterIndex++;
                        }
                    }
                }
            }
        }
    }
}
