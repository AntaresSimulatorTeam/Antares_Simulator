#include "antares/solver/optimisation/constraints/POutCapacityThreasholds.h"

void POutCapacityThreasholds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx
      = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

    if (!data.Simulation)
    {
        // 17 bis
        // Power output remains within limits set by minimum stable power and maximum capacity threasholds
        // l * M + Sum(P^on_re-) <= P <= u * M - Sum(P^on_re+) 
        // l : minimum stable power output when running 
        // u : maximum stable power output when running 
        // M : number of running units in cluster θ 
        // P^on_re- : Participation of running units in cluster θ to Down reserves 
        // P^on_re+ : Participation of running units in cluster θ to Up reserves 
        // P : Power output from cluster θ

        // 17 bis (1) : l * M + Sum(P^on_re-) - P <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING)
                        && (data.thermalClusters[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[reserveParticipations.clusterIdInArea] == globalClusterIdx))
                        builder.RunningClusterReserveParticipation(reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder
                  .NumberOfDispatchableUnits(
                    globalClusterIdx,
                    data.thermalClusters[pays].pminDUnGroupeDuPalierThermique[cluster])
                  .DispatchableProduction(globalClusterIdx, -1)
                  .lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThreasholdInf(
                  builder.data.nombreDeContraintes,
                  data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }

         // 17 bis (2) : P - u * M + Sum(P^on_re+) <= 0
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING) 
                        && (data.thermalClusters[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[reserveParticipations.clusterIdInArea] == globalClusterIdx))
                        builder.RunningClusterReserveParticipation(reserveParticipations.globalIndexClusterParticipation, 1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, 1)
                  .NumberOfDispatchableUnits(
                    globalClusterIdx,
                    -data.thermalClusters[pays].PmaxDUnGroupeDuPalierThermique[cluster])
                  .lessThan();
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThreasholdSup(
                  builder.data.nombreDeContraintes,
                  data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }
    }
    else
    {
        // Lambda that count the number of reserves that the cluster is participating to
        auto countReservesFromCluster
          = [cluster](const std::vector<CAPACITY_RESERVATION>& reservations, int globalClusterIdx, int pays, ReserveData data)
        {
            int counter = 0;
            for (const auto& capacityReservation : reservations)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if ((reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING)
                        && (data.thermalClusters[pays]
                             .NumeroDuPalierDansLEnsembleDesPaliersThermiques[reserveParticipations
                                                                                .clusterIdInArea]
                           == globalClusterIdx))
                        counter++;
                }
            }
            return counter;
        };

        int nbConstraintsToAdd
          = countReservesFromCluster(
              data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp, globalClusterIdx, pays, data)
            + countReservesFromCluster(
              data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown, globalClusterIdx, pays, data);

        builder.data.NbTermesContraintesPourLesReserves += 2*(nbConstraintsToAdd+2);

        builder.data.nombreDeContraintes += 2*nbConstraintsToAdd;
    }
}
