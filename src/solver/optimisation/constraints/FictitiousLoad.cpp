// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FictitiousLoad.h"

void FictitiousLoad::add(int pdt, int pays)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContraintePourEviterLesChargesFictives[pays]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);

    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.FictiveLoads(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt);
    ExportPaliers(data.PaliersThermiquesDuPays[pays], builder);

    if (data.DefaillanceNegativeUtiliserHydro[pays])
    {
        builder.HydProd(pays, -1.0);
        for (const auto& storage: data.ShortTermStorage[pays])
        {
            builder.ShortTermStorageWithdrawal(storage.clusterGlobalIndex, -1.0);
        }
    }
    builder.Spillage(pays, 1.0);

    builder.lessThan();
    builder.build();
}
