#include "antares/solver/optimisation/constraints/STStorageLevelReserveParticipation.h"

void STStorageLevelReserveParticipation::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (g) (1)
        // Participation of down reserves requires a sufficient level of stock
        // R_t + Sum(P_{res} * R_{min,res}) <= R_up
        // R_t : stock level at time t
        // P_{res} : power participation for reserve down res
        // R_{min,res} : max power participation ratio
        // R_up : max stock level

        // 15 (g) (2)
        // Participation of up reserves requires a sufficient level of stock
        // -R_t + Sum(P_{res} * R_{min,res}) <= -R_down
        // R_t : stock level at time t
        // P_{res} : power participation for reserve up res
        // R_{min,res} : max power participation ratio
        // R_down : min stock level
        for (auto type: {ReserveType::DOWN, ReserveType::UP})
        {
            builder.updateHourWithinWeek(pdt);

            for (auto& capacityReservation:
                 data.areaReserves[pays].areaCapacityReservations | filter(type))
            {
                if (capacityReservation.AllSTStorageReservesParticipation.contains(cluster))
                {
                    RESERVE_PARTICIPATION_STSTORAGE reserveParticipations
                      = capacityReservation.AllSTStorageReservesParticipation[cluster];
                    builder.STStorageClusterReserveParticipation(
                      capacityReservation.type,
                      reserveParticipations.globalIndexClusterParticipation,
                      capacityReservation.powerActivationRatio);
                }
            }
            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageLevel(globalClusterIdx,
                                              type == ReserveType::DOWN ? 1. : -1.);
                builder.lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .reservesIndices.value()
                  .STStorageLevelParticipation[type][globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STStorageLevelReserveParticipation(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name,
                  type);
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
