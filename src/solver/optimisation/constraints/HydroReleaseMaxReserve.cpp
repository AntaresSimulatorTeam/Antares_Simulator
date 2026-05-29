#include "antares/solver/optimisation/constraints/HydroReleaseMaxReserve.h"

void HydroReleaseMaxReserve::add(int pays, int reserve, int cluster, int pdt)
{
    if (!data.Simulation)
    {
        // 15 (a)
        // Participation to the reserve using release is bounded by the max release reserve
        // participation of the cluster constraint : H <= H^max H : Release power H^max : Maximum
        // accessible power of the cluster for the reserve

        CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                      .areaCapacityReservations[reserve];

        RESERVE_PARTICIPATION_HYDRO& reserveParticipation = capacityReservation
                                                              .AllHydroReservesParticipation
                                                                [cluster];

        builder.updateHourWithinWeek(pdt)
          .HydroReleaseReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                            1.0)
          .lessThan();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .reservesIndices.value()
          .HydroMaxReleaseParticipation[reserveParticipation.globalIndexClusterParticipation]
          = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.HydroReleaseMaxReserve(builder.data.nombreDeContraintes,
                                     reserveParticipation.clusterName,
                                     capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
