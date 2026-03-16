// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrMaxEnsLoad.h"

void CsrMaxEnsLoad::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        builder.updateHourWithinWeek(data.hour);

        builder.PositiveUnsuppliedEnergy(Area, 1.0);

        data.numberOfConstraintCsrMaxEnsLoad[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrMaxEnsLoad(builder.data.nombreDeContraintes);

        builder.lessThan();
        builder.build();
    }
}
