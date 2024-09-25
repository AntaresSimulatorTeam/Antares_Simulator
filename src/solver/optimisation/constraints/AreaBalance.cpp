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

#include "antares/solver/optimisation/constraints/AreaBalance.h"

static void shortTermStorageBalance(const ::ShortTermStorage::AREA_INPUT& shortTermStorageInput,
                                    ConstraintBuilder& constraintBuilder)
{
    for (const auto& storage: shortTermStorageInput)
    {
        unsigned index = storage.clusterGlobalIndex;
        constraintBuilder.ShortTermStorageInjection(index, 1.0)
          .ShortTermStorageWithdrawal(index, -1.0);
    }
}

void AreaBalance::add(int pdt, int pays)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays[pays]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);
    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.AreaBalance(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt);

    int interco = data.IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder.NTCDirect(interco, 1.0);
        interco = data.IndexSuivantIntercoOrigine[interco];
    }

    interco = data.IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.NTCDirect(interco, -1.0);
        interco = data.IndexSuivantIntercoExtremite[interco];
    }

    ExportPaliers(data.PaliersThermiquesDuPays[pays], builder);
    builder.HydProd(pays, -1.0)
      .Pumping(pays, 1.0)
      .PositiveUnsuppliedEnergy(pays, -1.0)
      .NegativeUnsuppliedEnergy(pays, 1.0);

    shortTermStorageBalance(data.ShortTermStorage[pays], builder);

    builder.equalTo();
    builder.build();
}
