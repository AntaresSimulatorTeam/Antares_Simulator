#include "HydroPowerSmoothingUsingVariationMaxUp.h"

void HydroPowerSmoothingUsingVariationMaxUp::add(int pays, int pdt)
{
    // if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
    // {
    //     return;
    // }

    ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);
    namer.UpdateArea(builder.data.NomsDesPays[pays]);
    namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
    namer.HydroPowerSmoothingUsingVariationMaxUp(builder.data.nombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .include(Variable::HydProd(pays), 1.0)
      .updateHourWithinWeek(0)
      .include(Variable::HydProdUp(pays), -1.0)
      .greaterThan()
      .build();
}