#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt, bool isUpReserve)
{
    CAPACITY_RESERVATION capacityReservation
      = isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp[reserve]
                    : data.areaReserves[pays].areaCapacityReservationsDown[reserve];

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDeBesoinEnReserves[capacityReservation.globalReserveIndex]
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
             cluster < capacityReservation.AllThermalReservesParticipation.size();
             cluster++)
        {
            if (capacityReservation.AllThermalReservesParticipation[cluster].maxPower
                != CLUSTER_NOT_PARTICIPATING)
                builder.RunningThermalClusterReserveParticipation(
                  capacityReservation.AllThermalReservesParticipation[cluster]
                    .globalIndexClusterParticipation,
                  1);
        }

        // Short Term Storage clusters reserve participation
        for (size_t cluster = 0;
             cluster < capacityReservation.AllSTStorageReservesParticipation.size();
             cluster++)
        {
            if ((capacityReservation.AllSTStorageReservesParticipation[cluster].maxTurbining
                 != CLUSTER_NOT_PARTICIPATING)
                || (capacityReservation.AllSTStorageReservesParticipation[cluster].maxPumping
                    != CLUSTER_NOT_PARTICIPATING))
            {
                if (isUpReserve)
                    builder.STStorageClusterReserveUpParticipation(
                      capacityReservation.AllSTStorageReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
                else
                    builder.STStorageClusterReserveDownParticipation(
                      capacityReservation.AllSTStorageReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
            }
        }

        // Long Term Storage clusters reserve participation
        for (size_t cluster = 0;
             cluster < capacityReservation.AllLTStorageReservesParticipation.size();
             cluster++)
        {
            if ((capacityReservation.AllLTStorageReservesParticipation[cluster].maxTurbining
                 != CLUSTER_NOT_PARTICIPATING)
                || (capacityReservation.AllLTStorageReservesParticipation[cluster].maxPumping
                    != CLUSTER_NOT_PARTICIPATING))
            {
                if (isUpReserve)
                    builder.LTStorageClusterReserveUpParticipation(
                      capacityReservation.AllLTStorageReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
                else
                    builder.LTStorageClusterReserveDownParticipation(
                      capacityReservation.AllLTStorageReservesParticipation[cluster]
                        .globalIndexClusterParticipation,
                      1);
            }
        }

        builder.InternalUnsatisfiedReserve(capacityReservation.globalReserveIndex, 1)
          .InternalExcessReserve(capacityReservation.globalReserveIndex, -1)
          .equalTo();
        data.CorrespondanceCntNativesCntOptim[pdt]
          .NumeroDeContrainteDesContraintesDeBesoinEnReserves[capacityReservation
                                                                .globalReserveIndex]
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
        for (size_t cluster = 0;
             cluster < capacityReservation.AllThermalReservesParticipation.size();
             cluster++)
        {
            if (capacityReservation.AllThermalReservesParticipation[cluster].maxPower
                != CLUSTER_NOT_PARTICIPATING)
                nbTermes++;
        }
        for (size_t cluster = 0;
             cluster < capacityReservation.AllSTStorageReservesParticipation.size();
             cluster++)
        {
            if ((capacityReservation.AllSTStorageReservesParticipation[cluster].maxTurbining
                 != CLUSTER_NOT_PARTICIPATING)
                || (capacityReservation.AllSTStorageReservesParticipation[cluster].maxPumping
                    != CLUSTER_NOT_PARTICIPATING))
                nbTermes++;
        }
        //for (size_t cluster = 0;
        //     cluster < capacityReservation.AllLTStorageReservesParticipation.size();
        //     cluster++)
        //{
        //    if ((capacityReservation.AllLTStorageReservesParticipation[cluster].maxTurbining
        //         != CLUSTER_NOT_PARTICIPATING)
        //        || (capacityReservation.AllLTStorageReservesParticipation[cluster].maxPumping
        //            != CLUSTER_NOT_PARTICIPATING))
        //        nbTermes++;
        //}

        builder.data.NbTermesContraintesPourLesReserves += 2 + nbTermes;

        builder.data.nombreDeContraintes += 1;
    }
}
