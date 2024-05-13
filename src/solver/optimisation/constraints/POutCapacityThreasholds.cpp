#include "antares/solver/optimisation/constraints/POutCapacityThreasholds.h"

void POutCapacityThreasholds::add(int pays, int cluster, int pdt)
{
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

        int globalClusterIdx = data.thermalClusters[pays]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

        // 17 bis (1) : l * M + Sum(P^on_re-) - P <= 0
        {
            builder.updateHourWithinWeek(pdt)
              .NumberOfDispatchableUnits(
                globalClusterIdx, data.thermalClusters[pays].pminDUnGroupeDuPalierThermique[cluster])
              .DispatchableProduction(globalClusterIdx, -1);

            for (const auto& capacityReservation :
                 data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if (reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING)
                        builder.RunningClusterReserveParticipation(globalClusterIdx, 1);
                }
            }

            builder.lessThan();

            if (builder.NumberOfVariables() > 0)
            {
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThreasholdInf(
                  builder.data.nombreDeContraintes,
                  data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
            }
            builder.build();
        }

         // 17 bis (2) : P - u * M + Sum(P^on_re+) <= 0
        {
            builder.updateHourWithinWeek(pdt)
              .DispatchableProduction(globalClusterIdx, 1)
              .NumberOfDispatchableUnits(
                globalClusterIdx, -data.thermalClusters[pays].PmaxDUnGroupeDuPalierThermique[cluster]);

            for (const auto& capacityReservation :
                 data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if (reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING)
                        builder.RunningClusterReserveParticipation(globalClusterIdx, 1);
                }
            }

            builder.lessThan();

            if (builder.NumberOfVariables() > 0)
            {
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutCapacityThreasholdSup(
                  builder.data.nombreDeContraintes,
                  data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
            }
            builder.build();
        }
    }
    else
    {
        // Lambda that count the number of reserves that the cluster is participating to
        auto countReservesFromCluster
          = [cluster](const std::vector<CAPACITY_RESERVATION>& reservations)
        {
            int counter = 0;
            for (const auto& capacityReservation : reservations)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllReservesParticipation)
                {
                    if (reserveParticipations.maxPower != CLUSTER_NOT_PARTICIPATING)
                        counter++;
                }
            }
            return counter;
        };

        int nbConstraintsToAdd
          = countReservesFromCluster(
              data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsUp)
            + countReservesFromCluster(
              data.areaReserves.thermalAreaReserves[pays].areaCapacityReservationsDown);

        builder.data.NbTermesContraintesPourLesReserves += 2*(nbConstraintsToAdd+2);

        builder.data.nombreDeContraintes += 2*nbConstraintsToAdd;
    }
}
