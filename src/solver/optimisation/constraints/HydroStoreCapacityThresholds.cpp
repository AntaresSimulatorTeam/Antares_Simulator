// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/HydroStoreCapacityThresholds.h"

void HydroStoreCapacityThresholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (d)
        // Store power remains within limits set by minimum stable power (0) and maximum capacity
        // thresholds Sum(Π^on_re+) <= Π <= Πmax - Sum(Π^on_re-) Π^on_re- : Store Participation
        // of cluster to Down reserves Π^on_re+ : Store Participation of cluster to Up reserves Π
        // : Store Power output from cluster Πmax : Maximum Store Power from cluster

        // 15 (d) (1) : Sum(Π^on_re+) - Π <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::UP))
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllHydroReservesParticipation)
                {
                    builder.HydroStoreReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.hydroOfArea[pays].PresenceDePompageModulable)
                {
                    builder.Pumping(pays, -1);
                }
                builder.lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroStoreCapacityThresholdsDown(builder.data.nombreDeContraintes, "Hydro");
                builder.build();
            }
        }

        // 15 (d) (2) :  Π + Sum(Π^on_re-) <= Πmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::DOWN))
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllHydroReservesParticipation)
                {
                    builder.HydroStoreReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.hydroOfArea[pays].PresenceDHydrauliqueModulable)
                {
                    builder.Pumping(pays, 1);
                }
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroStoreCapacityThresholds[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroStoreCapacityThresholdsUp(builder.data.nombreDeContraintes, "Hydro");
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForHydroReserves(pays);
    }
}
