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

#include "antares/solver/optimisation/constraints/ConsistenceNumberOfDispatchableUnits.h"

void ConsistenceNumberOfDispatchableUnits::add(int pays, int index, int pdt)
{
    if (!data.Simulation)
    {
        int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                      .NombreDePasDeTempsPourUneOptimisation;

        auto cluster = data.PaliersThermiquesDuPays[pays]
                         .NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
        int Pdtmoins1 = pdt - 1;
        if (Pdtmoins1 < 0)
        {
            Pdtmoins1 = NombreDePasDeTempsPourUneOptimisation + Pdtmoins1;
        }

        builder.updateHourWithinWeek(pdt)
          .NumberOfDispatchableUnits(cluster, 1.0)
          .updateHourWithinWeek(Pdtmoins1)
          .NumberOfDispatchableUnits(cluster, -1)
          .updateHourWithinWeek(pdt)
          .NumberStartingDispatchableUnits(cluster, -1)
          .NumberStoppingDispatchableUnits(cluster, 1)
          .equalTo();

        if (builder.NumberOfVariables() > 0)
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateArea(builder.data.NomsDesPays[pays]);

            namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
            namer.ConsistenceNODU(
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
