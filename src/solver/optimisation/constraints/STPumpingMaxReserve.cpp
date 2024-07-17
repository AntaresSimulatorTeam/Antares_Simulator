#include "antares/solver/optimisation/constraints/STPumpingMaxReserve.h"

void STPumpingMaxReserve::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 15 (l)
        // Participation to the reserve is bounded by the max pumping reserve participation of the
        // cluster constraint : Π <= Π^max Π : Pumping power Π^max : Maximum accessible pumping
        // limit of the cluster for the reserve

        CAPACITY_RESERVATION<RESERVE_PARTICIPATION_STSTORAGE>& capacityReservation
          = isUpReserve ? data.areaReserves.shortTermStorageAreaReserves[pays]
                            .areaCapacityReservationsUp[reserve]
                        : data.areaReserves.shortTermStorageAreaReserves[pays]
                            .areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_STSTORAGE& reserveParticipation
          = capacityReservation.AllReservesParticipation[cluster];

        int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

        builder.updateHourWithinWeek(pdt)
          .STStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesSTStorageClusterMaxInjectionParticipation
            [reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.STPumpingMaxReserve(builder.data.nombreDeContraintes,
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
