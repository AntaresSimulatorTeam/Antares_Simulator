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
        double multiplicateurStartingDispatchableUnits = -pmaxDUnGroupeDuPalierThermique
                                                         + maxUpwardPowerRampingRate;
        // 18 (bis)
        // Equation : P(t) <= P(t-1) - ((R^+) * M(t)) + (u * M^+(t))
        // P(t) : Power output of the cluster at timestep t
        // R^+ : Max ramping up for cluster
        // M(t) : number of running units
        // u : maximum unit power output when running
        // M^+(t) : number of starting units this timestep
        // constraint : P(t) - P(t-1) - ((R^+) * M(t)) - u * M^+(t) < 0

        // Modif : commentaire à merger au dessus
        // Equation : P(t) <= P(t-1) - ((R^+) * M(t-1)) + (u * M^+(t))
        // constraint : P(t) - P(t-1) - ((R^+) * M(t)) - ((u-R^+) * M^+(t)) < 0

        builder.updateHourWithinWeek(pdt)
          .DispatchableProduction(cluster, 1.0)
          .DispatchableProduction(cluster,
                                  -1.0,
                                  -1,
                                  builder.data.NombreDePasDeTempsPourUneOptimisation)
          .NumberOfDispatchableUnits(cluster, -maxUpwardPowerRampingRate)
          .NumberStartingDispatchableUnits(cluster, multiplicateurStartingDispatchableUnits)
          .lessThan();

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
