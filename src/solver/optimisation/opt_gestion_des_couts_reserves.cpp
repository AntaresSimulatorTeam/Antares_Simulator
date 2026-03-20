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
#include "antares/solver/optimisation/opt_gestion_des_couts_reserves.h"

#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/optimisation/variables/VariableManagerUtils.h"
#include "antares/solver/simulation/simulation.h"

void OPT_InitialiserLesCoutsLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                             const int PremierPdtDeLIntervalle,
                                             const int DernierPdtDeLIntervalle)
{
    struct ReserveCostsInitializer
    {
        PROBLEME_HEBDO* problemeHebdo;
        const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
        VariableManagement::VariableManager variableManager;
        std::vector<double>& CoutLineaire;
        int pdtHebdo;

        ReserveCostsInitializer(PROBLEME_HEBDO* hebdo):
            problemeHebdo(hebdo),
            ProblemeAResoudre(hebdo->ProblemeAResoudre),
            variableManager(VariableManagerFromProblemHebdo(hebdo)),
            CoutLineaire(hebdo->ProblemeAResoudre->CoutLineaire),
            pdtHebdo(0)
        {
        }

        void setPdtHebdo(int pdt)
        {
            pdtHebdo = pdt;
        }

        // Init costs for a reserve
        void initReserveCosts(const CAPACITY_RESERVATION& reserve)
        {
            int var = variableManager.InternalExcessReserve(reserve.globalReserveIndex, pdtHebdo);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                CoutLineaire[var] = reserve.spillageCost;
            }

            var = variableManager.InternalUnsatisfiedReserve(reserve.globalReserveIndex, pdtHebdo);
            if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
            {
                CoutLineaire[var] = reserve.unsuppliedCost;
            }
        }

        // Init costs for a Thermal cluster participation to a reserve up
        void initThermalReserveParticipationCosts(
          ReserveType type,
          const RESERVE_PARTICIPATION_THERMAL& reserveParticipation)
        {
            int var = variableManager.RunningThermalClusterReserveParticipation(
              reserveParticipation.globalIndexClusterParticipation,
              pdtHebdo);
            CoutLineaire[var] = reserveParticipation.participationCost;
            if (type == ReserveType::UP)
            {
                var = variableManager.OffThermalClusterReserveParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  pdtHebdo);
                CoutLineaire[var] = reserveParticipation.participationCostOff;
            }
        }

        // Init costs for a ShortTerm cluster participation to a reserve up
        void initSTStorageReserveParticipationCosts(
          ReserveType type,
          const RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation)
        {
            int var = variableManager.STStorageClusterReserveParticipation(
              type,
              reserveParticipation.globalIndexClusterParticipation,
              pdtHebdo);
            CoutLineaire[var] = reserveParticipation.participationCost;
        }

        // Init costs for a Hydro participation to a reserve
        void initHydroReserveParticipationCosts(
          ReserveType type,
          const RESERVE_PARTICIPATION_HYDRO& reserveParticipation)
        {
            int var = variableManager.HydroReserveParticipation(
              type,
              reserveParticipation.globalIndexClusterParticipation,
              pdtHebdo);
            CoutLineaire[var] = reserveParticipation.participationCost;
        }
    };

    ReserveCostsInitializer reserveCostsInitializer(problemeHebdo);

    for (int pdtHebdo = PremierPdtDeLIntervalle; pdtHebdo < DernierPdtDeLIntervalle; pdtHebdo++)
    {
        reserveCostsInitializer.setPdtHebdo(pdtHebdo);
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            auto areaReserves = problemeHebdo->allReserves.value()[pays];

            for (auto& areaReserve: areaReserves.areaCapacityReservations)
            {
                reserveCostsInitializer.initReserveCosts(areaReserve);

                // Thermal clusters
                for (const auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllThermalReservesParticipation)
                {
                    reserveCostsInitializer.initThermalReserveParticipationCosts(
                      areaReserve.type,
                      clusterReserveParticipation);
                }

                // Short Term Storage clusters
                for (const auto& [clusterId, clusterReserveParticipation]:
                     areaReserve.AllSTStorageReservesParticipation)
                {
                    reserveCostsInitializer.initSTStorageReserveParticipationCosts(
                      areaReserve.type,
                      clusterReserveParticipation);
                }

                // Hydro
                for (const auto& clusterReserveParticipation:
                     areaReserve.AllHydroReservesParticipation)
                {
                    reserveCostsInitializer.initHydroReserveParticipationCosts(
                      areaReserve.type,
                      clusterReserveParticipation);
                }
            }
        }
    }
}
