#include "antares/solver/optimisation/constraints/ThermalReserveParticipation.h"

void ThermalReserveParticipation::add(int pays, int reserve, int cluster, int pdt, bool isUpReserve)
{
    if (!data.Simulation)
    {
        // 17 quinquies / sexies
        // Participation to the reserve is the sum of off and on units reserve participations
        // constraint : P - P^on - P^off  = 0
        // P : Participation power
        // P^on : Participation of running units
        // P^off : Participation of off units

        CAPACITY_RESERVATION capacityReservation
          = isUpReserve ? data.areaReserves[pays].areaCapacityReservationsUp[reserve]
                        : data.areaReserves[pays].areaCapacityReservationsDown[reserve];

        auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation[cluster];

        int globalClusterIdx
          = data.thermalClusters[pays].NumeroDuPalierDansLEnsembleDesPaliersThermiques
              [reserveParticipation.clusterIdInArea];

        builder.updateHourWithinWeek(pdt)
          .ThermalClusterReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                              1.0)
          .RunningThermalClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation, -1.0);

        if (isUpReserve)
        {
            builder.OffThermalClusterReserveParticipation(
              reserveParticipation.globalIndexClusterParticipation, -1.0);
        }

        builder.equalTo();

        ConstraintNamer namer(builder.data.NomDesContraintes);
        const int hourInTheYear = builder.data.weekInTheYear * 168 + pdt;
        namer.UpdateTimeStep(hourInTheYear);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.ParticipationOfUnitsToReserve(builder.data.nombreDeContraintes,
                                            reserveParticipation.clusterName,
                                            capacityReservation.reserveName);
        builder.build();
    }
    else
    {
        builder.data.NbTermesContraintesPourLesReserves += isUpReserve ? 3 : 2;
        builder.data.nombreDeContraintes++;
    }
}
