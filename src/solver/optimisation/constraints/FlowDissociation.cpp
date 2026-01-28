// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FlowDissociation.h"

void FlowDissociation::add(int pdt, int interco)
{
    if (const COUTS_DE_TRANSPORT& CoutDeTransport = data.CoutDeTransport[interco];
        CoutDeTransport.IntercoGereeAvecDesCouts)
    {
        data.CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDeDissociationDeFlux[interco]
          = builder.data.nombreDeContraintes;
        const auto origin = builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[interco]];
        const auto destination = builder.data
                                   .NomsDesPays[data.PaysExtremiteDeLInterconnexion[interco]];
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.updateExtremities(origin, destination);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FlowDissociation(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(pdt);
        builder.NTCDirect(interco, 1.0)
          .IntercoDirectCost(interco, -1.0)
          .IntercoIndirectCost(interco, 1.0);

        builder.equalTo();

        builder.build();
    }
}
