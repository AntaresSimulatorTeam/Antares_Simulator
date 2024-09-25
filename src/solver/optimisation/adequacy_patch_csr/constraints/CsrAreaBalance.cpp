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

#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrAreaBalance.h"

void CsrAreaBalance::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        // + ENS
        builder.updateHourWithinWeek(data.hour).PositiveUnsuppliedEnergy(Area, 1.0);

        // - export flows
        int Interco = data.IndexDebutIntercoOrigine[Area];
        while (Interco >= 0)
        {
            if (data.extremityAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoOrigine[Interco];
                continue;
            }

            // flow (A->2)
            builder.NTCDirect(Interco, -1.0);

            logs.debug() << "S-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysExtremiteDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoOrigine[Interco];
        }

        // or + import flows
        Interco = data.IndexDebutIntercoExtremite[Area];
        while (Interco >= 0)
        {
            if (data.originAreaMode[Interco] != Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            {
                Interco = data.IndexSuivantIntercoExtremite[Interco];
                continue;
            }
            // flow (2 -> A)
            builder.NTCDirect(Interco, 1.0);

            logs.debug() << "E-Interco number: [" << std::to_string(Interco) << "] between: ["
                         << builder.data.NomsDesPays[Area] << "]-["
                         << builder.data.NomsDesPays[data.PaysOrigineDeLInterconnexion[Interco]]
                         << "]";

            Interco = data.IndexSuivantIntercoExtremite[Interco];
        }

        // - Spilled Energy
        builder.NegativeUnsuppliedEnergy(Area, -1.0);

        data.numberOfConstraintCsrAreaBalance[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrAreaBalance(builder.data.nombreDeContraintes);
        builder.equalTo();
        builder.build();
    }
}
