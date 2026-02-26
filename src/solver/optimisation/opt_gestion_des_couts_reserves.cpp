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
#include "antares/solver/optimisation/opt_reserves.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/simulation.h"

#include "variables/VariableManagerUtils.h"

namespace //anonymous
{

class ReserveCostsInitializer
{
public:
    explicit ReserveCostsInitializer(PROBLEME_HEBDO* hebdo):
        problemeHebdo(hebdo),
        ProblemeAResoudre(hebdo->ProblemeAResoudre),
        variableManager(VariableManagerFromProblemHebdo(hebdo)),
        CoutLineaire(hebdo->ProblemeAResoudre->CoutLineaire)
    {
    }

    void setPdtHebdo(int pdt)
    {
        pdtHebdo = pdt;
    }

    // Init costs for a reserve
    void initReserveCosts(const CAPACITY_RESERVATION& reserve)
    {
        setCostIfValid(variableManager.InternalExcessReserve(reserve.globalReserveIndex, pdtHebdo),
                       reserve.spillageCost);

        setCostIfValid(variableManager.InternalUnsatisfiedReserve(reserve.globalReserveIndex,
                                                                  pdtHebdo),
                       reserve.unsuppliedCost);
    }

    // Init costs for a Thermal cluster participation to a reserve up
    void initThermalReserveParticipationCosts(
      ReserveType type,
      const RESERVE_PARTICIPATION_THERMAL& reserveParticipation)
    {
        setCostIfValid(variableManager.RunningThermalClusterReserveParticipation(
                         reserveParticipation.globalIndexClusterParticipation,
                         pdtHebdo),
                       reserveParticipation.participationCost);

        if (type == ReserveType::UP)
        {
            setCostIfValid(variableManager.OffThermalClusterReserveParticipation(
                             reserveParticipation.globalIndexClusterParticipation,
                             pdtHebdo),
                           reserveParticipation.participationCostOff);
        }
    }

    // Init costs for a ShortTerm cluster participation to a reserve up
    void initSTStorageReserveParticipationCosts(
      ReserveType type,
      const RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation)
    {
        setCostIfValid(variableManager.STStorageClusterReserveParticipation(
                         type,
                         reserveParticipation.globalIndexClusterParticipation,
                         pdtHebdo),
                       reserveParticipation.participationCost);
    }

    // Init costs for a Hydro participation to a reserve
    void initHydroReserveParticipationCosts(ReserveType type,
                                            const RESERVE_PARTICIPATION_HYDRO& reserveParticipation)
    {
        setCostIfValid(variableManager.HydroReserveParticipation(
                         type,
                         reserveParticipation.globalIndexClusterParticipation,
                         pdtHebdo),
                       reserveParticipation.participationCost);
    }

private:
    void setCostIfValid(int var, double cost)
    {
        if (var >= 0 && var < ProblemeAResoudre->NombreDeVariables)
        {
            CoutLineaire[var] = cost;
        }
    }

    PROBLEME_HEBDO* problemeHebdo;
    const std::unique_ptr<PROBLEME_ANTARES_A_RESOUDRE>& ProblemeAResoudre;
    VariableManagement::VariableManager variableManager;
    std::vector<double>& CoutLineaire;
    int pdtHebdo = 0;
};

} // anonymous namespace

void OPT_InitialiserLesCoutsLineaireReserves(PROBLEME_HEBDO* problemeHebdo,
                                             int PremierPdtDeLIntervalle,
                                             int DernierPdtDeLIntervalle)
{
    ReserveCostsInitializer initializer(problemeHebdo);

    for (int pdt = PremierPdtDeLIntervalle; pdt < DernierPdtDeLIntervalle; ++pdt)
    {
        initializer.setPdtHebdo(pdt);

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; ++pays)
        {
            const auto& areaReserves = problemeHebdo->allReserves.value()[pays];

            for (const auto& areaReserve: areaReserves.areaCapacityReservations)
            {
                initializer.initReserveCosts(areaReserve);

                for (const auto& [_, participation]: areaReserve.AllThermalReservesParticipation)
                {
                    initializer.initThermalReserveParticipationCosts(areaReserve.type,
                                                                     participation);
                }

                for (const auto& [_, participation]: areaReserve.AllSTStorageReservesParticipation)
                {
                    initializer.initSTStorageReserveParticipationCosts(areaReserve.type,
                                                                       participation);
                }

                for (const auto& participation: areaReserve.AllHydroReservesParticipation)
                {
                    initializer.initHydroReserveParticipationCosts(areaReserve.type, participation);
                }
            }
        }
    }
}
