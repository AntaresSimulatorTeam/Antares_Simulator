#include "MaxPumping.h"

void MaxPumping::add(int pays)
{
    if (data.CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
    {
        data.NumeroDeContrainteMaxPompage[pays] = builder.data.nombreDeContraintes;

        for (int pdt = 0; pdt < builder.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.Pumping(pays, 1.0);
        }
        data.NumeroDeContrainteMaxPompage[pays] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.MaxPumping(builder.data.nombreDeContraintes);
        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxPompage[pays] = -1;
}