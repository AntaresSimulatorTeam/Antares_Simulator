// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/ExchangeBalance.h"

void ExchangeBalance::add(uint32_t pays)
{
    builder.updateHourWithinWeek(0);

    int interco = data.IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder.DirectFlux(interco, 1.0);
        interco = data.IndexSuivantIntercoOrigine[interco];
    }
    interco = data.IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.DirectFlux(interco, -1.0);

        interco = data.IndexSuivantIntercoExtremite[interco];
    }

    data.NumeroDeContrainteDeSoldeDEchange[pays] = builder.data.nombreDeContraintes;
    builder.equalTo();
    builder.build();
}
