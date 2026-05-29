#include "antares/solver/optimisation/constraints/STStoreMaxReserve.h"

void STStoreMaxReserve::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (l)
        // Participation to the reserve is bounded by the max store reserve participation of the
        // cluster Π <= Π^max Π : Store power Π^max : Maximum accessible limit of the cluster for
        // the reserve

        CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                      .areaCapacityReservations[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation = capacityReservation
                                                                  .AllSTStorageReservesParticipation
                                                                    [cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStorageStoreClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices.value()
          .STStorageClusterMaxStoreParticipation[reserveParticipation
                                                   .globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STStoreMaxReserve(builder.data.nombreDeContraintes,
                                reserveParticipation.clusterName,
                                capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
