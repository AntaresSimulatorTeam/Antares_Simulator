#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt, bool isUpReserve)
{
    CAPACITY_RESERVATION capacityReservation
      = isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp[reserve]
                    : data.areaReserves[pays].areaCapacityReservationsDown[reserve];

    if (!data.Simulation)
    {
        // 24

        // Sum(P_θ) + Sum(P_B) + Sum(P_H) = S + J^+ -J^-
        // P_θ : Participation power from thermal cluster θ to the reserve res
        // P_B : Participation power from Short Term Storage cluster B to the reserve res
        // P_H : Participation power from Long Term Storage to the reserve res
        // S : Internal reserve res need for the area (second membre)
        // J^+ : Amount of internal excess reserve for the reserve res
        // J^- : Amount of internal unsatisfied reserve for the reserve res

        builder.updateHourWithinWeek(pdt);

        // Thermal clusters reserve participation
        for (auto& reserveParticipation: capacityReservation.AllThermalReservesParticipation)
        {
            builder.ThermalClusterReserveParticipation(
              reserveParticipation.globalIndexClusterParticipation,
              1);
        }

        // Short Term Storage clusters reserve participation
        for (auto& reserveParticipation: capacityReservation.AllSTStorageReservesParticipation)
        {
            if (isUpReserve)
            {
                builder.STStorageClusterReserveUpParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }
            else
            {
                builder.STStorageClusterReserveDownParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }
        }

        // Long Term Storage clusters reserve participation
        for (auto& reserveParticipation: capacityReservation.AllLTStorageReservesParticipation)
        {
            if (isUpReserve)
            {
                builder.LTStorageClusterReserveUpParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }
            else
            {
                builder.LTStorageClusterReserveDownParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
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
        int nbTermes = capacityReservation.AllThermalReservesParticipation.size()
                       + capacityReservation.AllSTStorageReservesParticipation.size()
                       + capacityReservation.AllLTStorageReservesParticipation.size();
        if (nbTermes)
        {
            builder.data.NbTermesContraintesPourLesReserves += 2 + nbTermes;
            builder.data.nombreDeContraintes += 1;
        }
    }
}
