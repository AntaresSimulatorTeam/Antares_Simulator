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
            for (const auto& areaReserveUp : areaReserves.areaCapacityReservationsUp)
            {
                if (!Simulation)
                {
                    // For Unsatisfied Reserves
                    CorrespondanceVarNativesVarOptim.internalUnsatisfiedReserveIndex[reserveIndex]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalUnsatisfiedReserve(NombreDeVariables,
                                                             areaReserveUp.reserveName);

                    // For Excess Reserves
                    CorrespondanceVarNativesVarOptim.internalExcessReserveIndex[reserveIndex]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalExcessReserve(NombreDeVariables,
                                                        areaReserveUp.reserveName);
                }
                NombreDeVariables += 2;

                int clusterIndex = 0;
                for (const auto& clusterReserveParticipation :
                     areaReserveUp.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        const auto& clusterName
                          = PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex];
                        if (!Simulation)
                        {
                            // For running units in cluster
                            CorrespondanceVarNativesVarOptim
                              .runningClusterReserveParticipationIndex[index]
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfRunningUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveUp.reserveName);

                            // For all units in cluster (off units can participate to the reserves)
                            CorrespondanceVarNativesVarOptim.clusterReserveParticipationIndex[index]
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveUp.reserveName);
                            index++;
                            clusterIndex++;
                        }
                        NombreDeVariables += 2;
                    }
                }
            }
            for (const auto& areaReserveDown : areaReserves.areaCapacityReservationsDown)
            {
                if (!Simulation)
                {
                    // For Unsatisfied Reserves
                    CorrespondanceVarNativesVarOptim.internalUnsatisfiedReserveIndex[reserveIndex]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalUnsatisfiedReserve(NombreDeVariables,
                                                             areaReserveDown.reserveName);

                    // For Excess Reserves
                    CorrespondanceVarNativesVarOptim.internalExcessReserveIndex[reserveIndex]
                      = NombreDeVariables;
                    ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                      = VARIABLE_BORNEE_DES_DEUX_COTES;
                    variableNamer.InternalExcessReserve(NombreDeVariables,
                                                        areaReserveDown.reserveName);
                }
                NombreDeVariables += 2;

                int clusterIndex = 0;
                for (const auto& clusterReserveParticipation :
                     areaReserveDown.AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        const auto& clusterName
                          = PaliersThermiquesDuPays.NomsDesPaliersThermiques[clusterIndex];
                        if (!Simulation)
                        {
                            // For running units in cluster
                            CorrespondanceVarNativesVarOptim
                              .runningClusterReserveParticipationIndex[index]
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfRunningUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveDown.reserveName);

                            // For all units in cluster (off units can participate to the reserves)
                            CorrespondanceVarNativesVarOptim.clusterReserveParticipationIndex[index]
                              = NombreDeVariables;
                            ProblemeAResoudre->TypeDeVariable[NombreDeVariables]
                              = VARIABLE_BORNEE_DES_DEUX_COTES;
                            variableNamer.ParticipationOfUnitsToReserve(
                              NombreDeVariables, clusterName, areaReserveDown.reserveName);
                            index++;
                            clusterIndex++;
                        }
                        NombreDeVariables += 2;
                    }
                }
            }
        }
    }
}
