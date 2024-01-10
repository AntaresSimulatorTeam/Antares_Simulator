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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "MaxHydroPower.h"

void MaxHydroPower::add(int pays)
{
    if (data.CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable
        && (data.CaracteristiquesHydrauliques[pays].TurbinageEntreBornes
            || data.CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        for (int pdt = 0; pdt < builder.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.HydProd(pays, 1.0);
        }
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.MaxHydroPower(builder.data.nombreDeContraintes);

        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
}