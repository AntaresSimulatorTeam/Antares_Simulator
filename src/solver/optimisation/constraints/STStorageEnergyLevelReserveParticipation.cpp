// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/STStorageEnergyLevelReserveParticipation.h"

void STStorageEnergyLevelReserveParticipation::add(int pays, int cluster, int reserve, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;
    CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                  .areaCapacityReservations[reserve];

    if (capacityReservation.referenceActivationDuration > 0)
    {
        if (!data.Simulation)
        {
            // 15 (h) (1)
            // Participation of down reserves requires a sufficient level of stock
            //  Sum(P_{res,t_st} * R_{min,res} +/- J_res * R_{lambda,t_st}) <= n_min * R_up
            // R_t : stock level at time t
            // P_{res,t_st} : power participation for reserve down res at time t_st
            // R_{min,res} : max power participation ratio
            // R_up : max stock level
            {
                double sign = capacityReservation.type == ReserveType::UP ? -1.0 : 1.0;

                RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation
                  = capacityReservation.AllSTStorageReservesParticipation[cluster];

                builder.updateHourWithinWeek(pdt);

                for (int t = 0; t < capacityReservation.referenceActivationDuration; t++)
                {
                    builder.STStorageClusterReserveParticipation(
                      capacityReservation.type,
                      reserveParticipation.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio,
                      t,
                      builder.data.NombreDePasDeTempsPourUneOptimisation);

                    builder.ShortTermStorageLevel(
                      globalClusterIdx,
                      sign * capacityReservation.energyActivationRatio,
                      t,
                      builder.data.NombreDePasDeTempsPourUneOptimisation);
                }

                builder.lessThan();

                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageEnergyLevelParticipation[reserveParticipation
                                                       .globalIndexClusterParticipation]
                  = builder.data.nombreDeContraintes;

                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STEnergyStockLevelReserveParticipation(builder.data.nombreDeContraintes,
                                                             reserveParticipation.clusterName,
                                                             capacityReservation.reserveName);
                builder.build();
            }
        }
        else
        {
            builder.data.nombreDeContraintes += 1;
        }
    }
}
