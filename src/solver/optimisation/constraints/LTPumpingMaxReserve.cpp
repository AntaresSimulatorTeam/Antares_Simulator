#include "antares/solver/optimisation/constraints/LTPumpingMaxReserve.h"

void LTPumpingMaxReserve::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
         //15 (l)
         //Participation to the reserve is bounded by the max pumping reserve participation of the cluster
         //Π <= Π^max 
         //Π : Pumping power 
         //Π^max : Maximum accessible limit of the cluster for the reserve

        CAPACITY_RESERVATION& capacityReservation
          = isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp[reserve]
                        : data.areaReserves[pays].areaCapacityReservationsDown[reserve];

        RESERVE_PARTICIPATION_LTSTORAGE& reserveParticipation
          = capacityReservation.AllLTStorageReservesParticipation[cluster];

        builder.updateHourWithinWeek(pdt)
          .LTStoragePumpingClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, 1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesLTStorageClusterMaxInjectionParticipation
            [reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.LTPumpingMaxReserve(builder.data.nombreDeContraintes,
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
