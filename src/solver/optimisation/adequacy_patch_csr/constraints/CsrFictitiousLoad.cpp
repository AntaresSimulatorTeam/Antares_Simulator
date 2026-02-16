#include "CsrFictitiousLoad.h"

void CsrFictitiousLoad::add()
{
    for (uint32_t Area = 0; Area < data.NombreDePays; ++Area)
    {
        if (data.areaMode[Area] != Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
            continue;

        builder.updateHourWithinWeek(data.hour);
        
        // Add all thermal dispatchable generation with coefficient -1.0
        // This represents STt in the formula
        for (int index = 0; index < data.PaliersThermiquesDuPays[Area].NombreDePaliersThermiques; index++)
        {
            const int palier = data.PaliersThermiquesDuPays[Area].NumeroDuPalierDansLEnsembleDesPaliersThermiques[index];
            builder.DispatchableProduction(palier, -1.0);
        }
        
        // Add hydro production if enabled
        // This represents Ht in the formula
        auto hydroCoeff = data.DefaillanceNegativeUtiliserHydro[Area] ? -1.0 : 0.0;
        if (data.DefaillanceNegativeUtiliserHydro[Area])
        {
            builder.HydProd(Area, hydroCoeff);
        }
        
        // Add spillage (negative unsupplied energy) with coefficient 1.0
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
