#include "antares/solver/optimisation/constraints/STTurbiningCapacityThreasholds.h"

void STTurbiningCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx = data.shortTermStorageOfArea[pays][cluster].clusterGlobalIndex;

    if (!data.Simulation)
    {
        // 15 (m)
        // Turbining power remains within limits set by minimum stable power (0) and maximum capacity threasholds 
        // Hmin + Sum(H^on_re-) <= H <= Hmax - Sum(H^on_re+) 
        // H^on_re- : Turbining Participation of cluster to Down reserves 
        // H^on_re+ : Turbining Participation of cluster to Up reserves 
        // H : Turbining Power output from cluster 
        // Hmax : Maximum Turbining Power from cluster

        // 15 (m) (1) : H - Sum(H^on_re-) >= Hmin
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllSTStorageReservesParticipation)
                {
                        builder.STStorageTurbiningClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, -1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).greaterThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                    .NumeroDeContrainteDesContraintesSTStorageClusterTurbiningCapacityThreasholdsMin
                    [globalClusterIdx]
                    = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STTurbiningCapacityThreasholdsDown(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }

        // 15 (m) (2) :  H + Sum(H^on_re+) <= Hmax
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllSTStorageReservesParticipation)
                {
                        builder.STStorageTurbiningClusterReserveParticipation(
                          reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.ShortTermStorageWithdrawal(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesSTStorageClusterTurbiningCapacityThreasholdsMax
                    [globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.STTurbiningCapacityThreasholdsUp(
                  builder.data.nombreDeContraintes,
                  data.shortTermStorageOfArea[pays][cluster].name);
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserveParticipations
        auto countReservesParticipations = [](const std::vector<CAPACITY_RESERVATION>& reservations)
        {
            int counter = 0;
            for (const auto& capacityReservation: reservations)
            {
                counter += capacityReservation.AllSTStorageReservesParticipation.size();
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