/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/NbDispUnitsMinBoundSinceMinUpTime.h"

void NbDispUnitsMinBoundSinceMinUpTime::add(int pays, int index, int pdt)
{
    auto cluster = data.PaliersThermiquesDuPays[pays]
                     .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
    const int DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
      = data.PaliersThermiquesDuPays[pays].DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[index];

    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
      = -1;
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                      .NombreDePasDeTempsPourUneOptimisation;

        builder.updateHourWithinWeek(pdt).NumberOfDispatchableUnits(cluster, 1.0);

        for (int k = pdt - DureeMinimaleDeMarcheDUnGroupeDuPalierThermique + 1; k <= pdt; k++)
        {
            int t1 = k;
            if (t1 < 0)
            {
                t1 = NombreDePasDeTempsPourUneOptimisation + t1;
            }

            builder.updateHourWithinWeek(t1)
              .NumberStartingDispatchableUnits(cluster, -1.0)
              .NumberBreakingDownDispatchableUnits(cluster, 1.0);
        }

        builder.greaterThan();
        if (builder.NumberOfVariables() > 1)
        {
            data.CorrespondanceCntNativesCntOptim[pdt]
              .NumeroDeContrainteDesContraintesDeDureeMinDeMarche[cluster]
              = builder.data.nombreDeContraintes;

            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.NbDispUnitsMinBoundSinceMinUpTime(
              builder.data.nombreDeContraintes,
              data.PaliersThermiquesDuPays[pays].NomsDesPaliersThermiques[index]);
            builder.build();
        }
    }
    else
    {
        builder.data.nombreDeContraintes++;
    }
}
