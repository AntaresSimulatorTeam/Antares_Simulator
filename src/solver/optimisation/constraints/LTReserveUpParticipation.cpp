#include "antares/solver/optimisation/constraints/LTReserveUpParticipation.h"

void LTReserveUpParticipation::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (e)
        // Participation to the up reserve is the sum of the turbining and pumping participation
        // constraint : P_res = H_res + Π_res
        // H : Turbining participation to reserve
        // Π : Pumping participation to reserve
        // P : Up Reserve Participation

        CAPACITY_RESERVATION& capacityReservation
          = data.areaReserves[pays].areaCapacityReservationsUp[reserve];

        RESERVE_PARTICIPATION_LTSTORAGE& reserveParticipation
          = capacityReservation.AllLTStorageReservesParticipation[cluster];

        builder.updateHourWithinWeek(pdt)
          .LTStorageTurbiningClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0)
          .LTStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0)
          .LTStorageClusterReserveUpParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.LTReserveUpParticipation(builder.data.nombreDeContraintes,
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
