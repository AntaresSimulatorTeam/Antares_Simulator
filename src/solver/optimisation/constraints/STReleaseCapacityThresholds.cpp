#include "antares/solver/optimisation/constraints/STReleaseCapacityThresholds.h"

void STReleaseCapacityThresholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (m)
        // Release power remains within limits set by minimum stable power (0) and maximum
        // capacity thresholds Hmin + Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) H^on_re- :
        // Release Participation of cluster to Down reserves H^on_re+ : Release Participation of
        // cluster to Up reserves H : Release Power output from cluster Hmax : Maximum Release
        // Power from cluster Hmin : Minimal release of power from cluster

        // 15 (m) (1) : H - Sum(H^on_re-) >= Hmin
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::DOWN))
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageReleaseClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      -1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).greaterThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageClusterReleaseCapacityThresholdsMin[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STReleaseCapacityThresholdsDown(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (m) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::UP))
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageReleaseClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageClusterReleaseCapacityThresholdsMax[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STReleaseCapacityThresholdsUp(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        builder.data.nombreDeContraintes += data.countNumberOfConstraintsForSTStorageReserves(
          pays,
          cluster);
    }
}
