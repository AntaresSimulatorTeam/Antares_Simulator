#include "antares/solver/optimisation/constraints/LTTurbiningCapacityThreasholds.h"

void LTTurbiningCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.longTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (c)
        // Turbining power remains within limits set by minimum stable power (0) and maximum capacity threasholds 
        // Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) 
        // H^on_re- : Turbining Participation of cluster to Down reserves 
        // H^on_re+ : Turbining Participation of cluster to Up reserves 
        // H : Turbining Power output from cluster 
        // Hmax : Maximum Turbining Power from cluster

        // 15 (c) (1) : Sum(H^on_re-) - H <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllLTStorageReservesParticipation)
                {
                    if ((reserveParticipations.maxTurbining != CLUSTER_NOT_PARTICIPATING)
                        && (data.longTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        builder.LTStorageTurbiningClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.LongTermStorageWithdrawal(globalClusterIdx, -1).lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.LTTurbiningCapacityThreasholds(
                  builder.data.nombreDeContraintes,
                  data.longTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (c) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllLTStorageReservesParticipation)
                {
                    if ((reserveParticipations.maxTurbining != CLUSTER_NOT_PARTICIPATING)
                        && (data.longTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        builder.LTStorageTurbiningClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.LongTermStorageWithdrawal(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesSTStorageClusterTurbiningCapacityThreasholds
                    [globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.LTTurbiningCapacityThreasholds(
                  builder.data.nombreDeContraintes,
                  data.longTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserves that the cluster is participating to
        auto countReservesFromCluster
          = [cluster](const std::vector<CAPACITY_RESERVATION>& reservations,
                      int globalClusterIdx,
                      int pays,
                      ReserveData data)
        {
            int counter = 0;
            for (const auto& capacityReservation : reservations)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllLTStorageReservesParticipation)
                {
                    if ((reserveParticipations.maxTurbining != CLUSTER_NOT_PARTICIPATING)
                        && (data.longTermStorageOfArea[pays][reserveParticipations.clusterIdInArea]
                              .clusterGlobalIndex
                            == globalClusterIdx))
                        counter++;
                }
            }
            return counter;
        };

        int nbTermsUp = countReservesFromCluster(
          data.areaReserves[pays].areaCapacityReservationsUp, globalClusterIdx, pays, data);
        int nbTermsDown = countReservesFromCluster(
          data.areaReserves[pays].areaCapacityReservationsDown, globalClusterIdx, pays, data);

        builder.data.NbTermesContraintesPourLesReserves
          += (nbTermsUp + 1) * (nbTermsUp > 0) + (nbTermsDown + 1) * (nbTermsDown > 0);

        builder.data.nombreDeContraintes += (nbTermsUp > 0) + (nbTermsDown > 0);
    }
}