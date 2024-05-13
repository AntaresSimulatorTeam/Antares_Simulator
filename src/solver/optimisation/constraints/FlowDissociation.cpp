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
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FlowDissociation(builder.data.nombreDeContraintes, origin, destination);

        builder.updateHourWithinWeek(pdt);
        builder.NTCDirect(interco, 1.0)
          .IntercoDirectCost(interco, -1.0)
          .IntercoIndirectCost(interco, 1.0);

        builder.equalTo();

        builder.build();
    }
}
