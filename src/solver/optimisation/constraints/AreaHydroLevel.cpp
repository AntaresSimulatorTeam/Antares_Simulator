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
#include "AreaHydroLevel.h"

void AreaHydroLevel::add(int pays, int pdt)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays]
      = builder.data.nombreDeContraintes;
    if (data.CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
    {
        builder.updateHourWithinWeek(pdt).HydroLevel(pays, 1.0);
        if (pdt > 0)
        {
            builder.updateHourWithinWeek(pdt - 1).HydroLevel(pays, -1.0);
        }
        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.AreaHydroLevel(builder.data.nombreDeContraintes);
        data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays]
          = builder.data.nombreDeContraintes;
        builder.updateHourWithinWeek(pdt)
          .HydProd(pays, 1.0)
          .Pumping(pays, -data.CaracteristiquesHydrauliques[pays].PumpingRatio)
          .Overflow(pays, 1.)
          .equalTo()
          .build();
    }
    else
        data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesNiveauxPays[pays] = -1;
}