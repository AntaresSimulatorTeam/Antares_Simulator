#include "MinHydroPower.h"

void MinHydroPower::add(int pays, std::shared_ptr<MinHydroPowerData> data)
{
    if (data->presenceHydro && (data->TurbEntreBornes || data->PresenceDePompageModulable))
    {
        data->NumeroDeContrainteMinEnergieHydraulique[pays] = builder->data->nombreDeContraintes;

        ConstraintNamer namer(builder->data->NomDesContraintes);
        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear);
        namer.MinHydroPower(builder->data->nombreDeContraintes);

        for (int pdt = 0; pdt < data->NombreDePasDeTempsPourUneOptimisation; pdt++)
        {
            builder->updateHourWithinWeek(pdt);
            builder->HydProd(pays, 1.0);
        }

        data->NumeroDeContrainteMinEnergieHydraulique[pays] = builder->data->nombreDeContraintes;
        builder->greaterThan().build();
    }
    else
        data->NumeroDeContrainteMinEnergieHydraulique[pays] = -1;
}
