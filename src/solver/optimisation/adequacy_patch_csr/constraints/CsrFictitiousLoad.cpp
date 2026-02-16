#include "CsrFictitiousLoad.h"

void CsrFictitiousLoad::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            continue;

        // Add spilled energy (negative unsupplied energy) with coefficient 1.0
        // In CSR context, this is equivalent to the original FictitiousLoad constraint
        // but without thermal and hydro variables since those are fixed in CSR
        builder.updateHourWithinWeek(data.hour).NegativeUnsuppliedEnergy(Area, 1.0);

        data.numberOfConstraintCsrFictitiousLoad[Area] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(data.hour);
        namer.UpdateArea(builder.data.NomsDesPays[Area]);
        namer.CsrFictitiousLoad(builder.data.nombreDeContraintes);
        
        builder.lessThan();
        builder.build();
    }
}
