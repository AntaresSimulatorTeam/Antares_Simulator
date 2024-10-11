#include "antares/solver/optimisation/constraints/POutBounds.h"

void POutBounds::add(int pays, int cluster, int pdt)
{
    int globalClusterIdx
      = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

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


        // 17 ter (1) : Sum(P^on_re-) - P <= - P_down
        {
            builder.updateHourWithinWeek(pdt);

            for (const auto& capacityReservation :
                 data.areaReserves[pays].areaCapacityReservationsDown)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllThermalReservesParticipation)
                {
                    builder.RunningThermalClusterReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, -1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[globalClusterIdx]
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
                 data.areaReserves[pays].areaCapacityReservationsUp)
            {
                for (const auto& reserveParticipations :
                     capacityReservation.AllThermalReservesParticipation)
                {
                    builder.RunningThermalClusterReserveParticipation(
                      reserveParticipations.globalIndexClusterParticipation,
                      1);
                }
            }

            if (builder.NumberOfVariables() > 0)
            {
                builder.DispatchableProduction(globalClusterIdx, 1).lessThan();
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMinDuPalier[globalClusterIdx]
                  = builder.data.nombreDeContraintes;
                data.CorrespondanceCntNativesCntOptim[pdt]
                  .NumeroDeContrainteDesContraintesDePuissanceMaxDuPalier[globalClusterIdx]
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
        // Lambda that count the number of reserves Participations
        auto countReservesParticipations
          = [](const std::vector<CAPACITY_RESERVATION>& reservations)
        {
            int counter = 0;
            for (const auto& capacityReservation : reservations)
            {
                        counter += capacityReservation.AllThermalReservesParticipation.size();
            }
            return counter;
        };

        int nbTermsToAdd
          = countReservesParticipations(
              data.areaReserves[pays].areaCapacityReservationsUp)
            + countReservesParticipations(
              data.areaReserves[pays].areaCapacityReservationsDown);



        builder.data.NbTermesContraintesPourLesReserves += 2 * (nbTermsToAdd + 1);

        builder.data.nombreDeContraintes += 2;
    }
}
