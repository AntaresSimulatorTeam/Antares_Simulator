#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt, bool isUpReserve)
{
    CAPACITY_RESERVATION capacityReservation
      = isUpReserve
          ? data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp[reserve]
          : data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown[reserve];

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDeBesoinEnReserves[capacityReservation.globalReserveIndex]
      = -1;
    if (!data.Simulation)
    {
        // 24

        // Sum(P_θ) = S + J^+ -J^-
        // P_θ : Participation power from cluster θ to the reserve res
        // S : Internal reserve res need for the area (second membre)
        // J^+ : Amount of internal excess reserve for the reserve res
        // J^- : Amount of internal unsatisfied reserve for the reserve res

        builder.updateHourWithinWeek(pdt);

        for (size_t cluster = 0; cluster < capacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if (capacityReservation.AllReservesParticipation[cluster].maxPower != CLUSTER_NOT_PARTICIPATING)
                builder.RunningClusterReserveParticipation(
                  capacityReservation.AllReservesParticipation[cluster].globalIndexClusterParticipation,
                  1);
        }

        builder.InternalUnsatisfiedReserve(capacityReservation.globalReserveIndex, 1)
          .InternalExcessReserve(capacityReservation.globalReserveIndex, -1)
          .equalTo();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesDeBesoinEnReserves[capacityReservation.globalReserveIndex]
          = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.ReserveSatisfaction(builder.data.nombreDeContraintes,
                                  capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        int nbTermes = 0;
        for (size_t cluster = 0; cluster < capacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if (capacityReservation.AllReservesParticipation[cluster].maxPower
                != CLUSTER_NOT_PARTICIPATING)
                nbTermes++;
        }

        builder.data.NbTermesContraintesPourLesReserves += 2 + nbTermes;

        builder.data.nombreDeContraintes += 1;
    }
}
