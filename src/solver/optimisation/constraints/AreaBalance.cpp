// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/AreaBalance.h"

static void shortTermStorageBalance(const ::AREA_INPUT& shortTermStorageInput,
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
        builder.DirectFlow(interco, 1.0);
        interco = data.IndexSuivantIntercoOrigine[interco];
    }

    interco = data.IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.DirectFlow(interco, -1.0);
        interco = data.IndexSuivantIntercoExtremite[interco];
    }

    ExportPaliers(data.PaliersThermiquesDuPays[pays], builder);
    builder.HydProd(pays, -1.0).Pumping(pays, 1.0).UnsuppliedEnergy(pays, -1.0).Spillage(pays, 1.0);

    shortTermStorageBalance(data.ShortTermStorage[pays], builder);

    builder.equalTo();
    builder.build();
}
