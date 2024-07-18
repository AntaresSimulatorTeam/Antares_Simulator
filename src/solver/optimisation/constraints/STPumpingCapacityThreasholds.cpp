#include "antares/solver/optimisation/constraints/STPumpingCapacityThreasholds.h"

void STPumpingCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (n)
        // Pumping power remains within limits set by minimum stable power (0) and maximum capacity threasholds 
        // Sum(Π^on_re+) <= Π <= Πmax - Sum(Π^on_re-) 
        // Π^on_re- : Pumping Participation of cluster to Down reserves 
        // Π^on_re+ : Pumping Participation of cluster to Up reserves 
        // Π : Pumping Power output from cluster 
        // Πmax : Maximum Pumping Power from cluster

        // 15 (n) (1) : Sum(Π^on_re-) - Π <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves.shortTermStorageAreaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPumping != CLUSTER_NOT_PARTICIPATING)
                        && (data.shortTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        builder.STStoragePumpingClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageInjection(globalClusterIdx, -1).lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STPumpingCapacityThreasholds(builder.data.nombreDeContraintes,
                                                   data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (n) (2) :  Π + Sum(Π^on_re+) <= Πmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves.shortTermStorageAreaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPumping != CLUSTER_NOT_PARTICIPATING)
                        && (data.shortTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        builder.STStoragePumpingClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageInjection(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesSTStorageClusterPumpingCapacityThreasholds
                    [globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STPumpingCapacityThreasholds(builder.data.nombreDeContraintes,
                                                   data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserves that the cluster is participating to
        auto countReservesFromCluster =
          [cluster](
            const std::vector<CAPACITY_RESERVATION<RESERVE_PARTICIPATION_STSTORAGE>>& reservations,
            int globalClusterIdx,
            int pays,
            ReserveData data)
        {
            int counter = 0;
            for (const auto& capacityReservation : reservations)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPumping != CLUSTER_NOT_PARTICIPATING)
                        && (data.shortTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        counter++;
                }
            }
            return counter;
        };

        int nbConstraintsToAdd
          = countReservesFromCluster(
              data.areaReserves.shortTermStorageAreaReserves[pays].areaCapacityReservationsUp,
              globalClusterIdx,
              pays,
              data)
            + countReservesFromCluster(
              data.areaReserves.shortTermStorageAreaReserves[pays].areaCapacityReservationsDown,
              globalClusterIdx,
              pays,
              data);

        builder.data.NbTermesContraintesPourLesReserves += 2 * (nbConstraintsToAdd + 1);

        builder.data.nombreDeContraintes += 2 * nbConstraintsToAdd;
    }
}