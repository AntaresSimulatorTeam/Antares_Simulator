#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt, bool isUpReserve)
{
    CAPACITY_RESERVATION thermalCapacityReservation
      = isUpReserve
          ? data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp[reserve]
          : data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown[reserve];

    CAPACITY_RESERVATION STStorageCapacityReservation
      = isUpReserve
          ? data.areaReserves.shortTermStorageAreaReserves[pays].areaCapacityReservationsUp[reserve]
          : data.areaReserves.shortTermStorageAreaReserves[pays]
              .areaCapacityReservationsDown[reserve];

    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesContraintesDeBesoinEnReserves
      [thermalCapacityReservation.globalReserveIndex]
      = -1;
    if (!data.Simulation)
    {
        // 24

        // Sum(P_θ) + Sum(P_B) = S + J^+ -J^-
        // P_θ : Participation power from thermal cluster θ to the reserve res
        // P_B : Participation power from Short Term Storage cluster B to the reserve res
        // S : Internal reserve res need for the area (second membre)
        // J^+ : Amount of internal excess reserve for the reserve res
        // J^- : Amount of internal unsatisfied reserve for the reserve res

        builder.updateHourWithinWeek(pdt);

        // Thermal clusters reserve participation
        for (size_t cluster = 0;
             cluster < thermalCapacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if (thermalCapacityReservation.AllReservesParticipation[cluster].maxPower
                != CLUSTER_NOT_PARTICIPATING)
                builder.RunningThermalClusterReserveParticipation(
                  thermalCapacityReservation.AllReservesParticipation[cluster]
                    .globalIndexClusterParticipation,
                  1);
        }

        // Short Term Storage clusters reserve participation
        for (size_t cluster = 0;
             cluster < STStorageCapacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if ((STStorageCapacityReservation.AllReservesParticipation[cluster].maxTurbining
                 != CLUSTER_NOT_PARTICIPATING)
                || (STStorageCapacityReservation.AllReservesParticipation[cluster].maxPumping
                    != CLUSTER_NOT_PARTICIPATING))
            {
                if (isUpReserve)
                    builder.STStorageClusterReserveUpParticipation(
                      STStorageCapacityReservation.AllReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
                else
                    builder.STStorageClusterReserveDownParticipation(
                      STStorageCapacityReservation.AllReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
            }
        }

        builder.InternalUnsatisfiedReserve(thermalCapacityReservation.globalReserveIndex, 1)
          .InternalExcessReserve(thermalCapacityReservation.globalReserveIndex, -1)
          .equalTo();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesDeBesoinEnReserves[thermalCapacityReservation
                                                                .globalReserveIndex]
          = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.ReserveSatisfaction(builder.data.nombreDeContraintes,
                                  thermalCapacityReservation.reserveName);
        builder.build();
    }
    else
    {
        int nbTermes = 0;
        for (size_t cluster = 0;
             cluster < thermalCapacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if (thermalCapacityReservation.AllReservesParticipation[cluster].maxPower
                != CLUSTER_NOT_PARTICIPATING)
                nbTermes++;
        }
        for (size_t cluster = 0;
             cluster < STStorageCapacityReservation.AllReservesParticipation.size();
             cluster++)
        {
            if ((STStorageCapacityReservation.AllReservesParticipation[cluster].maxTurbining
                 != CLUSTER_NOT_PARTICIPATING)
                || (STStorageCapacityReservation.AllReservesParticipation[cluster].maxPumping
                    != CLUSTER_NOT_PARTICIPATING))
                nbTermes++;
        }

        builder.data.NbTermesContraintesPourLesReserves += 2 + nbTermes;

        builder.data.nombreDeContraintes += 1;
    }
}
