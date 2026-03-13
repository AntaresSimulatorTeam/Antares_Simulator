#include "CsrFictitiousLoad.h"

void CsrFictitiousLoad::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            continue;

        builder.updateHourWithinWeek(data.hour);

        // Only add spillage variable (negative unsupplied energy) with coefficient 1.0
        // Thermal and hydro production are constants in CSR, moved to RHS
        builder.NegativeUnsuppliedEnergy(Area, 1.0);

        data.numberOfConstraintCsrFictitiousLoad[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrFictitiousLoad(builder.data.nombreDeContraintes);

        builder.lessThan();
        builder.build();
    }
}
