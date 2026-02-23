#include "antares/solver/optimisation/constraints/STReserveParticipation.h"

void STReserveParticipation::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (o & p)
        // Participation to the up reserve is the sum of the release and store participation
        // constraint : P_res = H_res + Π_res
        // H : Release participation to reserve
        // Π : Store participation to reserve
        // P : Up Reserve Participation

        const auto& capacityReservation = data.areaReserves[pays].areaCapacityReservations[reserve];

        const auto& reserveParticipation = capacityReservation.AllSTStorageReservesParticipation.at(
          cluster);

        const int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;
        builder.updateHourWithinWeek(pdt)
          .STStorageReleaseClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            capacityReservation.type == ReserveType::UP ? -1.0 : 1.0)
          .STStorageStoreClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            capacityReservation.type == ReserveType::UP ? -1.0 : 1.0)
          .STStorageClusterReserveParticipation(
            capacityReservation.type,
            reserveParticipation.globalIndexClusterParticipation,
            capacityReservation.type == ReserveType::UP ? 1.0 : -1.0)
          .equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STReserveParticipation(builder.data.nombreDeContraintes,
                                     reserveParticipation.clusterName,
                                     capacityReservation.reserveName,
                                     capacityReservation.type);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
