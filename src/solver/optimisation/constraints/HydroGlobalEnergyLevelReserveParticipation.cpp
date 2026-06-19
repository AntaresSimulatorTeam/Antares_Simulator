// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroGlobalEnergyLevelReserveParticipation.h"

void HydroGlobalEnergyLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (t)
        // Stock participation is energy constrained (optional constraints)
        //  Sum(P_{res,t_st} * R_{min,res} +/- J_down/up * R_{lambda,t_st}) <= n_min * R_up
        // R_t : stock level at time t
        // P_{res,t_st} : power participation for reserve down res at time t_st
        // R_{min,res} : max power participation ratio
        // R_up : max stock level

        for (ReserveType type: {ReserveType::DOWN, ReserveType::UP})
        {
            builder.updateHourWithinWeek(pdt);
            int timeMax = data.areaReserves[pays].referenceGlobalActivationDuration[type];
            double maxGlobalEnergyActivationRatio = (type == ReserveType::UP ? -1 : 1)
                                                    * data.areaReserves[pays]
                                                        .maxGlobalEnergyActivationRatio[type];
            for (int t = 0; t < timeMax; t++)
            {
                for (auto& capacityReservation:
                     data.areaReserves[pays].areaCapacityReservations | filter(type))
                {
                    if (capacityReservation.AllHydroReservesParticipation.size())
                    {
                        RESERVE_PARTICIPATION_HYDRO& reserveParticipation
                          = capacityReservation.AllHydroReservesParticipation[cluster];
                        builder.HydroReserveParticipation(
                          type,
                          reserveParticipation.globalIndexClusterParticipation,
                          capacityReservation.powerActivationRatio,
                          t,
                          builder.data.NombreDePasDeTempsPourUneOptimisation);
                    }
                }
                if (builder.NumberOfVariables() > 0)
                {
                    builder.HydroLevel(globalClusterIdx, maxGlobalEnergyActivationRatio);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.lessThan();

                if (type == ReserveType::UP)
                {
                    data.CorrespondanceCntNativesCntOptim[pdt]
                      .reservesIndices.value()
                      .HydroGlobalEnergyLevelParticipationUp[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }
                else
                {
                    data.CorrespondanceCntNativesCntOptim[pdt]
                      .reservesIndices.value()
                      .HydroGlobalEnergyLevelParticipationDown[globalClusterIdx]
                      = builder.data.nombreDeContraintes;
                }

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                if (type == ReserveType::UP)
                {
                    namer.HydroGlobalEnergyLevelReserveParticipationUp(
                      builder.data.nombreDeContraintes,
                      "Hydro");
                }
                else
                {
                    namer.HydroGlobalEnergyLevelReserveParticipationDown(
                      builder.data.nombreDeContraintes,
                      "Hydro");
                }
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForHydroReserves(
          pays,
          true /*account For Global Activation Duration*/);
        ;
    }
}
