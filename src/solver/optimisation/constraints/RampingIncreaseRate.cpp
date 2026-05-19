// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/RampingIncreaseRate.h"

void RampingIncreaseRate::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        uint cluster = data.PaliersThermiquesDuPays[pays]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        double maxUpwardPowerRampingRate = data.PaliersThermiquesDuPays[pays]
                                             .maxUpwardPowerRampingRate[index];
        double pmaxDUnGroupeDuPalierThermique = data.PaliersThermiquesDuPays[pays]
                                                  .PmaxDUnGroupeDuPalierThermique[index];
        double pminDUnGroupeDuPalierThermique = data.PaliersThermiquesDuPays[pays]
                                                  .pminDUnGroupeDuPalierThermique[index];
        // 18 (bis)
        // Equation : P(t) <= P(t-1) - ((R^+) * (M(t) - M^+(t))) + (u * M^+(t)) - (l * M^-(t))
        // P(t) : Power output of the cluster at timestep t
        // R^+ : Max ramping up for cluster
        // M(t) : number of running units
        // u : maximum unit power output when running
        // l : minimum unit power output when running
        // M^+(t) : number of starting units this timestep
        // M^-(t) : number of stopping units this timestep
        // constraint : P(t) - P(t-1) - ((R^+) * M(t)) - ((u - R^+) * M^+(t)) + (l * M^-(t)) < 0

        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0) // P(t)
          .DispatchableProduction(cluster,
                                  -1.0,
                                  -1,
                                  builder.data.NombreDePasDeTempsPourUneOptimisation) // - P(t-1)
          .NumberOfDispatchableUnits(cluster, -maxUpwardPowerRampingRate) // - ((R^+) * M(t))
          .NumberStartingDispatchableUnits(
            cluster,
            maxUpwardPowerRampingRate - pmaxDUnGroupeDuPalierThermique) // ((u - R^+) * M^+(t))
          .NumberStoppingDispatchableUnits(cluster,
                                           pminDUnGroupeDuPalierThermique) // + (l * M^-(t))
          .lessThan();                                                     // < 0

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);
            namer.RampingIncreaseRate(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);
        }
        builder.build();
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
