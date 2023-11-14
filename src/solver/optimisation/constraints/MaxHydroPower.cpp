#include "MaxHydroPower.h"

void MaxHydroPower::add(int pays)
{
    if (data.CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable
        && (data.CaracteristiquesHydrauliques[pays].TurbinageEntreBornes
            || data.CaracteristiquesHydrauliques[pays].PresenceDePompageModulable))
    {
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        for (int pdt = 0; pdt < builder.data.NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder.updateHourWithinWeek(pdt);
            builder.HydProd(pays, 1.0);
        }
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear);
        namer.MaxHydroPower(builder.data.nombreDeContraintes);

        builder.lessThan().build();
    }
    else
        data.NumeroDeContrainteMaxEnergieHydraulique[pays] = -1;
}