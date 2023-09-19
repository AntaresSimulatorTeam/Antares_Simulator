#include "HydroPowerSmoothingUsingVariationSum.h"

void HydroPowerSmoothingUsingVariationSum::add(int pays,
                                               const int nombreDePasDeTempsPourUneOptimisation)
{
    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int pdt1 = pdt + 1;
        if (pdt1 >= nombreDePasDeTempsPourUneOptimisation)
            pdt1 = 0;
        ConstraintNamer namer(builder->data->NomDesContraintes, builder->data->NamedProblems);
        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.HydroPowerSmoothingUsingVariationSum(builder->data->nombreDeContraintes);

        builder->updateHourWithinWeek(pdt)
          .include(NewVariable::HydProd(pays), 1.0)
          .updateHourWithinWeek(pdt1) /* /!\ Re-check*/
          .include(NewVariable::HydProd(pays), -1.0)
          .updateHourWithinWeek(pdt) /* /!\ Re-check*/
          .include(NewVariable::HydProdDown(pays), -1.0)
          .include(NewVariable::HydProdUp(pays), 1.0)
          .equalTo()
          .build();
    }
}