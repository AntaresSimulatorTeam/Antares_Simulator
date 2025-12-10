#include "antares/solver/optimisation/constraints/HydroReleaseCapacityThresholds.h"

void HydroReleaseCapacityThresholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.hydroOfArea[pays].GlobalHydroIndex;

    if (!data.Simulation)
    {
        // 15 (c)
        // Release power remains within limits set by minimum stable power (0) and maximum
        // capacity thresholds Hmin + Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) H^on_re- :
        // Release Participation of cluster to Down reserves H^on_re+ : Release Participation of
        // cluster to Up reserves H : Release Power output from cluster Hmax : Maximum Release
        // Power from cluster

        // 15 (c) (1) : H - Sum(H^on_re-) >= Hmin
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::DOWN))
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllHydroReservesParticipation)
                {
                    builder.HydroReleaseReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      -1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.hydroOfArea[pays].PresenceDHydrauliqueModulable)
                {
                    builder.HydProd(pays, 1);
                }
                builder.greaterThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroReleaseCapacityThresholdsMin[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroReleaseCapacityThresholdsDown(builder.data.nombreDeContraintes, "Hydro");
                builder.build();
            }
        }

        // 15 (c) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::UP))
            {
                for (const auto& reserveParticipations:
                     capacityReservation.AllHydroReservesParticipation)
                {
                    builder.HydroReleaseReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                if (data.hydroOfArea[pays].PresenceDHydrauliqueModulable)
                {
                    builder.HydProd(pays, 1);
                }
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .HydroReleaseCapacityThresholdsMax[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.HydroReleaseCapacityThresholdsUp(builder.data.nombreDeContraintes, "Hydro");
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForHydroReserves(pays);
    }
}
