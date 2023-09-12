#include "HydroPowerSmoothingUsingVariationMaxDown.h"

void HydroPowerSmoothingUsingVariationMaxDown::add(int pays, int pdt)
{
    // if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    // {
    //     return;
    // }

    ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxDown(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .include(Variable::HydProd(pays), 1.0)
      .updateHourWithinWeek(0)
      .include(Variable::HydProdDown(pays), -1.0)
      .lessThan()
      .build();
}