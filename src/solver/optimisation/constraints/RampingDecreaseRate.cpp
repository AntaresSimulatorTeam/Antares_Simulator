// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/RampingDecreaseRate.h"

void RampingDecreaseRate::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        uint cluster = data.PaliersThermiquesDuPays[pays]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        double maxDownwardPowerRampingRate = data.PaliersThermiquesDuPays[pays]
                                               .maxDownwardPowerRampingRate[index];
        double pmaxDUnGroupeDuPalierThermique = data.PaliersThermiquesDuPays[pays]
                                                  .PmaxDUnGroupeDuPalierThermique[index];
        // 18 (ter)
        // Equation : P(t) >= P(t-1) - (R^- * M(t)) - (u * M^-(t))
        // P(t) : Power output of the cluster at timestep t
        // R^- : Max ramping down for cluster
        // M(t) : number of running units
        // u : maximum unit power output when running
        // M^-(t) : number of stopping units this timestep
        // constraint : P(t) - P(t-1) + R^- * M(t) + u * M^-(t) > 0

        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .DispatchableProduction(cluster,
                                  -1.0,
                                  -1,
                                  builder.data.NombreDePasDeTempsPourUneOptimisation)
          .NumberOfDispatchableUnits(cluster, maxDownwardPowerRampingRate)
          .NumberStoppingDispatchableUnits(cluster, pmaxDUnGroupeDuPalierThermique)
          .greaterThan();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.RampingDecreaseRate(
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
