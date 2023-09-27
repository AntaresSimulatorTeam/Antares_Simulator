#include "HydroPower.h"

void HydroPower::add(int pays, std::shared_ptr<HydroPowerData> data)
{
    const int NombreDePasDeTempsPourUneOptimisation
      = builder->data->NombreDePasDeTempsPourUneOptimisation;
    if (data->presenceHydro && !data->TurbEntreBornes)
    {
        if (data->presencePompage)
        {
            data->NumeroDeContrainteEnergieHydraulique[pays] = builder->data->nombreDeContraintes;

            const double pumpingRatio = data->pumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder->updateHourWithinWeek(pdt).HydProd(pays, 1.0).Pumping(pays, -pumpingRatio);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder->updateHourWithinWeek(pdt).HydProd(pays, 1.0);
            }
        }
        data->NumeroDeContrainteEnergieHydraulique[pays] = builder->data->nombreDeContraintes;

        builder->equalTo();
        ConstraintNamer namer(builder->data->NomDesContraintes);
        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear);
        namer.HydroPower(builder->data->nombreDeContraintes);
        builder->build();
    }
    else
        data->NumeroDeContrainteEnergieHydraulique[pays] = -1;
}
