// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ReserveSatisfaction.h"

void ReserveSatisfaction::add(int pays, int reserve, int pdt)
{
    CAPACITY_RESERVATION& capacityReservation = data.areaReserves[pays]
                                                  .areaCapacityReservations[reserve];

    int nbTermes = capacityReservation.AllThermalReservesParticipation.size()
                   + capacityReservation.AllSTStorageReservesParticipation.size()
                   + capacityReservation.AllHydroReservesParticipation.size();
    if (nbTermes)
    {
        if (!data.Simulation)
        {
            // 24

            // Sum(P_θ) + Sum(P_B) + Sum(P_H) = S + J^+ -J^-
            // P_θ : Participation power from thermal cluster θ to the reserve res
            // P_B : Participation power from Short Term Storage cluster B to the reserve res
            // P_H : Participation power from Hydro to the reserve res
            // S : Internal reserve res need for the area (second membre)
            // J^+ : Amount of internal excess reserve for the reserve res
            // J^- : Amount of internal unsatisfied reserve for the reserve res

            builder.updateHourWithinWeek(pdt);

            // Thermal clusters reserve participation
            for (auto& [clusterId, reserveParticipation]:
                 capacityReservation.AllThermalReservesParticipation)
            {
                builder.ThermalClusterReserveParticipation(
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }

            // Short Term Storage clusters reserve participation
            for (auto& [clusterId, reserveParticipation]:
                 capacityReservation.AllSTStorageReservesParticipation)
            {
                builder.STStorageClusterReserveParticipation(
                  capacityReservation.type,
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }

            // Hydro reserve participation
            for (auto& reserveParticipation: capacityReservation.AllHydroReservesParticipation)
            {
                builder.HydroReserveParticipation(
                  capacityReservation.type,
                  reserveParticipation.globalIndexClusterParticipation,
                  1);
            }

            builder.InternalUnsatisfiedReserve(capacityReservation.globalReserveIndex, 1)
              .InternalExcessReserve(capacityReservation.globalReserveIndex, -1)
              .equalTo();
            data.CorrespondanceCntNativesCntOptim[pdt]
              .reservesIndices.value()
              .need[capacityReservation.globalReserveIndex]
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
            builder.data.nombreDeContraintes += 1;
        }
    }
}
