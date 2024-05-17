#include "antares/solver/optimisation/constraints/POutBounds.h"

void POutBounds::add(int pays, int cluster, int pdt)
{
    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[cluster]
      = -1;
    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[cluster]
      = -1;

    if (!data.Simulation)
    {
        // 17 ter
        // Power output is bounded by must-run commitments and power availability, reserves must fit within the bounds 
        // P_down + Sum(P^on_re-) <= P <= P_up - Sum(P^on_re+)
        // P^on_re- : Participation of running units in cluster θ to Down reserves
        // P^on_re+ : Participation of running units in cluster θ to Up reserves
        // P : Power output from cluster θ
        // P_down : Minimal power output demanded from cluster θ 
        // P_up : Maximal power output from cluster θ

        int globalClusterIdx
          = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

        // 17 ter (1) : Sum(P^on_re-) - P <= - P_down
        {
            builder.updateHourWithinWeek(pdt);

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

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, -1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[cluster]
                  = builder.data.nombreDeContraintes;
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[cluster]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutBoundMin(builder.data.nombreDeContraintes,
                                   data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
        }

        // 17 ter (2) : P + Sum(P^on_re+) <= P_up
        {
            builder.updateHourWithinWeek(pdt);

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

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[cluster]
                  = builder.data.nombreDeContraintes;
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[cluster]
                  = builder.data.nombreDeContraintes;
                ConstraintNamer namer(builder.data.NomDesContraintes);
                const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
                namer.UpdateTimeStep(hourInTheYear);
                namer.UpdateArea(builder.data.NomsDesPays[pays]);
                namer.POutBoundMax(builder.data.nombreDeContraintes,
                                   data.thermalClusters[pays].NomsDesPaliersThermiques[cluster]);
                builder.build();
            }
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

        builder.data.NbTermesContraintesPourLesReserves += 2 * (nbConstraintsToAdd + 1);

        builder.data.nombreDeContraintes += 2 * nbConstraintsToAdd;
    }
}
