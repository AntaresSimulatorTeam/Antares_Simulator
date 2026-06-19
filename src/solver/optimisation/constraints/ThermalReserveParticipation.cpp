// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ThermalReserveParticipation.h"

void ThermalReserveParticipation::add(int pays, int reserve, int cluster, int pdt)
{
    CAPACITY_RESERVATION capacityReservation = data.areaReserves[pays]
                                                 .areaCapacityReservations[reserve];

    auto& reserveParticipation = capacityReservation.AllThermalReservesParticipation[cluster];
    bool offUnitParticipating = capacityReservation.type == ReserveType::UP
                                && reserveParticipation.maxPowerOff > 0;
    if (!data.Simulation)
    {
        // 17 quinquies / sexies
        // Participation to the reserve is the sum of off and on units reserve participations
        // constraint : P - P^on - P^off  = 0
        // P : Participation power
        // P^on : Participation of running units
        // P^off : Participation of off units

        int globalClusterIdx = data.thermalClusters[pays]
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster];

        builder.updateHourWithinWeek(pdt)
          .ThermalClusterReserveParticipation(reserveParticipation.globalIndexClusterParticipation,
                                              1.0)
          .RunningThermalClusterReserveParticipation(
            reserveParticipation.globalIndexClusterParticipation,
            -1.0);

        if (offUnitParticipating)
        {
            builder.OffThermalClusterReserveParticipation(
              reserveParticipation.globalIndexClusterParticipation,
              -1.0);
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
        builder.data.nombreDeContraintes++;
    }
}
