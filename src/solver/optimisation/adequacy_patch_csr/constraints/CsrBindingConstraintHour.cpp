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

#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrBindingConstraintHour.h"

void CsrBindingConstraintHour::add(int CntCouplante)
{
    if (data.MatriceDesContraintesCouplantes[CntCouplante].TypeDeContrainteCouplante
        != CONTRAINTE_HORAIRE)
    {
        return;
    }

    int NbInterco = data.MatriceDesContraintesCouplantes[CntCouplante]
                      .NombreDInterconnexionsDansLaContrainteCouplante;
    builder.updateHourWithinWeek(data.hour);

    for (int Index = 0; Index < NbInterco; Index++)
    {
        int Interco = data.MatriceDesContraintesCouplantes[CntCouplante]
                        .NumeroDeLInterconnexion[Index];
        double Poids = data.MatriceDesContraintesCouplantes[CntCouplante]
                         .PoidsDeLInterconnexion[Index];

        if (data.originAreaMode[Interco] == Data::AdequacyPatch::physicalAreaInsideAdqPatch
            && data.extremityAreaMode[Interco] == Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            builder.NTCDirect(Interco, Poids);
        }
    }

    if (builder.NumberOfVariables()
        > 0) // current binding constraint contains an interco type 2<->2
    {
        data.numberOfConstraintCsrHourlyBinding[CntCouplante] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.CsrBindingConstraintHour(
          builder.data.nombreDeContraintes,
          data.MatriceDesContraintesCouplantes[CntCouplante].NomDeLaContrainteCouplante);
        builder.SetOperator(
          data.MatriceDesContraintesCouplantes[CntCouplante].SensDeLaContrainteCouplante);
        builder.build();
    }
}
