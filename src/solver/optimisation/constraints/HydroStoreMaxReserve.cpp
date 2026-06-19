// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroStoreMaxReserve.h"

void HydroStoreMaxReserve::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (b)
        // Participation to the reserve is bounded by the max store reserve participation of the
        // cluster Π <= Π^max Π : Store power Π^max : Maximum accessible limit of the cluster for
        // the reserve

        CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                      .areaCapacityReservations[reserve];

        RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                              .AllHydroReservesParticipation
                                                                [cluster];

        builder.updateHourWithinWeek(pdt)
          .HydroStoreReserveParticipation(reserveParticipation.globalIndexClusterParticipation, 1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices.value()
          .HydroMaxStoreParticipation[reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.HydroStoreMaxReserve(builder.data.nombreDeContraintes,
                                   reserveParticipation.clusterName,
                                   capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
