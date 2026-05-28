#include "antares/solver/optimisation/constraints/STStoreCapacityThresholds.h"

void STStoreCapacityThresholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (n)
        // Store power remains within limits set by minimum stable power (0) and maximum capacity
        // thresholds Sum(Π^on_re+) <= Π <= Πmax - Sum(Π^on_re-) Π^on_re- : Store Participation
        // of cluster to Down reserves Π^on_re+ : Store Participation of cluster to Up reserves Π
        // : Store Power output from cluster Πmax : Maximum Store Power from cluster

        // 15 (n) (1) : Sum(Π^on_re+) - Π <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::UP))
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageStoreClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageInjection(globalClusterIdx, -1).lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STStoreCapacityThresholdsDown(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (n) (2) :  Π + Sum(Π^on_re-) <= Πmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(ReserveType::DOWN))
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    auto& reserveParticipation = capacityReservation
                                                   .AllSTStorageReservesParticipation.at(cluster);
                    builder.STStorageStoreClusterReserveParticipation(
                      reserveParticipation.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageInjection(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageClusterStoreCapacityThresholds[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STStoreCapacityThresholdsUp(builder.data.nombreDeContraintes,
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
