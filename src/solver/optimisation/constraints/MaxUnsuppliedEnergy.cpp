// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MaxUnsuppliedEnergy.h"

void MaxUnsuppliedEnergy::add(unsigned pdt, int pays)
{
    data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContraintePourBornerLaDefaillance[pays]
      = builder.data.nombreDeContraintes;

    ConstraintNamer namer(builder.data.NomDesContraintes);

    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.MaxUnsuppliedEnergy(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt);
    builder.UnsuppliedEnergy(pays, 1.0);

    builder.lessThan();
    builder.build();
}
