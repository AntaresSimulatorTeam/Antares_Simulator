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

#include "antares/solver/optimisation/constraints/HydroPower.h"

void HydroPower::add(int pays)
{
    const int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                        .NombreDePasDeTempsPourUneOptimisation;
    const auto& caracteristiquesHydrauliques = data.CaracteristiquesHydrauliques[pays];
    if (caracteristiquesHydrauliques.PresenceDHydrauliqueModulable
        && !caracteristiquesHydrauliques.TurbinageEntreBornes)
    {
        if (caracteristiquesHydrauliques.PresenceDePompageModulable)
        {
            data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

            const double pumpingRatio = caracteristiquesHydrauliques.PumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt)
                  .HydProd(pays, 1.0)
                  .Pumping(pays, -pumpingRatio)
                  .Overflow(pays, 1.0);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt).HydProd(pays, 1.0).Overflow(pays, 1.0);
            }
        }
        data.NumeroDeContrainteEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        builder.equalTo();
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.HydroPower(builder.data.nombreDeContraintes);
        builder.build();
    }
    else
    {
        data.NumeroDeContrainteEnergieHydraulique[pays] = -1;
    }
}
