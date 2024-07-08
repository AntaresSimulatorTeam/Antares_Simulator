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

#include "variables/VariableManagerUtils.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

#include "antares/solver/optimisation/opt_fonctions.h"

void OPT_InitialiserLesCoutsLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                                     const int PremierPdtDeLIntervalle,
                                                     const int DernierPdtDeLIntervalle)
{
    const auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;
    std::vector<double>& CoutLineaire = ProblemeAResoudre->CoutLineaire;
    auto variableManager = VariableManagerFromProblemHebdo(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle;
         pdtHebdo++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            auto reservesDuPays = problemeHebdo->allReserves.thermalAreaReserves[pays];

            const PALIERS_THERMIQUES& PaliersThermiquesDuPays
              = problemeHebdo->PaliersThermiquesDuPays[pays];
            int var;

            

            for (int index = 0; index < reservesDuPays.areaCapacityReservationsUp.size(); index++)
            {
                for (const auto& clusterReserveParticipation :
                     reservesDuPays.areaCapacityReservationsUp[index].AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        var = variableManager.ClusterReserveParticipation(
                          clusterReserveParticipation.globalIndexClusterParticipation,
                          pdtHebdo);
                        CoutLineaire[var] = clusterReserveParticipation.participationCost;
                    }
                }

                var = variableManager.InternalExcessReserve(
                  reservesDuPays.areaCapacityReservationsUp[index].globalReserveIndex, pdtHebdo);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = reservesDuPays.areaCapacityReservationsUp[index].spillageCost;
                }

               var = variableManager.InternalUnsatisfiedReserve(
                  reservesDuPays.areaCapacityReservationsUp[index].globalReserveIndex, pdtHebdo);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = CoutLineaire[var]
                      = reservesDuPays.areaCapacityReservationsUp[index].failureCost;
                }
            }

            for (int index = 0; index < reservesDuPays.areaCapacityReservationsDown.size(); index++)
            {
                for (const auto& clusterReserveParticipation :
                     reservesDuPays.areaCapacityReservationsDown[index].AllReservesParticipation)
                {
                    if (clusterReserveParticipation.maxPower >= 0)
                    {
                        var = variableManager.ClusterReserveParticipation(
                          clusterReserveParticipation.globalIndexClusterParticipation, pdtHebdo);
                        CoutLineaire[var] = clusterReserveParticipation.participationCost;
                    }
                }

                var = variableManager.InternalExcessReserve(
                  reservesDuPays.areaCapacityReservationsDown[index].globalReserveIndex, pdtHebdo);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var]
                      = reservesDuPays.areaCapacityReservationsDown[index].spillageCost;
                }

                var = variableManager.InternalUnsatisfiedReserve(
                  reservesDuPays.areaCapacityReservationsDown[index].globalReserveIndex, pdtHebdo);
                if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
                {
                    CoutLineaire[var] = CoutLineaire[var]
                      = reservesDuPays.areaCapacityReservationsDown[index].failureCost;
                }
            }
        }
    }
}
