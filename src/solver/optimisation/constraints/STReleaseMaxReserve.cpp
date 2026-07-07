// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/STReleaseMaxReserve.h"

void STReleaseMaxReserve::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (k)
        // Participation to the reserve using release is bounded by the max release reserve
        // participation of the cluster constraint : H <= H^max H : Release power H^max : Maximum
        // accessible power of the cluster for the reserve

        CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                      .areaCapacityReservations[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation = capacityReservation
                                                                  .AllSTStorageReservesParticipation
                                                                    [cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStorageReleaseClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices.value()
          .STStorageClusterMaxReleaseParticipation[reserveParticipation
                                                     .globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STReleaseMaxReserve(builder.data.nombreDeContraintes,
                                  reserveParticipation.clusterName,
                                  capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
