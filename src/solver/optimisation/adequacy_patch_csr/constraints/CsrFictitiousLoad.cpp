// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrFictitiousLoad.h"

void CsrFictitiousLoad::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            continue;
        }

        builder.updateHourWithinWeek(data.hour);

        // Add spillage variable with coefficient +1.0
        // Constraint: spillage <= RHS
        // Where RHS = STt - (1-BT)*STmint + BH*Ht + BF*Max(0, Ft - Lt)
        // The RHS is computed in setRHSfictitiousLoadValue() based on results from
        // the first linear optimization step (thermal gen, hydro, etc.)
        builder.Spillage(Area, 1.0);

        data.numberOfConstraintCsrFictitiousLoad[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrFictitiousLoad(builder.data.nombreDeContraintes);

        builder.lessThan();
        builder.build();
    }
}
