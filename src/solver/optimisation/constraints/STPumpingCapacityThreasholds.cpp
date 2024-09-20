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

        // 15 (n) (1) : Sum(Π^on_re+) - Π <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllSTStorageReservesParticipation)
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
                namer.STPumpingCapacityThreasholdsDown(builder.data.nombreDeContraintes,
                                                   data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (n) (2) :  Π + Sum(Π^on_re-) <= Πmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllSTStorageReservesParticipation)
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
                namer.STPumpingCapacityThreasholdsUp(builder.data.nombreDeContraintes,
                                                   data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserves that the cluster is participating to
        auto countReservesParticipations = [](const std::vector<CAPACITY_RESERVATION>& reservations)
        {
            int counter = 0;
            for (const auto& capacityReservation: reservations)
            {
                counter += capacityReservation.AllLTStorageReservesParticipation.size();
            }
            return counter;
        };

        int nbTermsUp = countReservesParticipations(
          data.areaReserves[pays].areaCapacityReservationsUp);
        int nbTermsDown = countReservesParticipations(
          data.areaReserves[pays].areaCapacityReservationsDown);

        builder.data.NbTermesContraintesPourLesReserves
          += (nbTermsUp + 1) * (nbTermsUp > 0) + (nbTermsDown + 1) * (nbTermsDown > 0);

        builder.data.nombreDeContraintes += (nbTermsUp > 0) + (nbTermsDown > 0);
    }
}