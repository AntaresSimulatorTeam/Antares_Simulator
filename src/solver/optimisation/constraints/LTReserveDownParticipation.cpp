#include "antares/solver/optimisation/constraints/LTReserveDownParticipation.h"

void LTReserveDownParticipation::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (p)
        // Participation to the down reserve is the sum of the turbining and pumping participation
        // constraint : P_res = H_res + Π_res
        // H : Turbining participation to reserve
        // Π : Pumping participation to reserve
        // P : Down Reserve Participation

        CAPACITY_RESERVATION& capacityReservation
          = data.areaReserves[pays].areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_LTSTORAGE& reserveParticipation
          = capacityReservation.AllLTStorageReservesParticipation[cluster];

        builder.updateHourWithinWeek(pdt)
          .LTStorageTurbiningClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .LTStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .LTStorageClusterReserveDownParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.LTReserveDownParticipation(builder.data.nombreDeContraintes,
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