#include "antares/solver/optimisation/constraints/STTurbiningMaxReserve.h"

void STTurbiningMaxReserve::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (k)
        // Participation to the reserve using turbining is bounded by the max turbining reserve participation of the cluster
        // constraint : H <= H^max
        // H : Turbining power
        // H^max : Maximum accessible power of the cluster for the reserve

        CAPACITY_RESERVATION& capacityReservation
          = isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp[reserve]
                        : data.areaReserves[pays].areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation
          = capacityReservation.AllSTStorageReservesParticipation[cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStorageTurbiningClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesSTStorageClusterMaxWithdrawParticipation
            [reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STTurbiningMaxReserve(builder.data.nombreDeContraintes,
                                    reserveParticipation.clusterName,
                                    capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves++;
        builder.data.nombreDeContraintes++;
    }
}