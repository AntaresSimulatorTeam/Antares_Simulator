#include "antares/solver/optimisation/constraints/STReserveUpParticipation.h"

void STReserveUpParticipation::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (o)
        // Participation to the up reserve is the sum of the turbining and pumping participation
        // constraint : P_res = H_res + Π_res
        // H : Turbining participation to reserve
        // Π : Pumping participation to reserve
        // P : Up Reserve Participation

        CAPACITY_RESERVATION& capacityReservation
          = data.areaReserves[pays].areaCapacityReservationsUp[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation
          = capacityReservation.AllSTStorageReservesParticipation[cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStorageTurbiningClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0)
          .STStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0)
          .STStorageClusterReserveUpParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STReserveUpParticipation(builder.data.nombreDeContraintes,
                                       reserveParticipation.clusterName,
                                       capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += 3;
        builder.data.nombreDeContraintes++;
    }
}
