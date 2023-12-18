#include "FinalStockEquivalent.h"

void FinalStockEquivalent::add(int pays)
{
    const auto pdt = builder.data.NombreDePasDeTempsPourUneOptimisation - 1;
    if (data.CaracteristiquesHydrauliques[pays].AccurateWaterValue
        && data.CaracteristiquesHydrauliques[pays].DirectLevelAccess)
    { /*  equivalence constraint : StockFinal- Niveau[T]= 0*/

        data.NumeroDeContrainteEquivalenceStockFinal[pays] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FinalStockEquivalent(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .FinalStorage(pays, 1.0)
          .updateHourWithinWeek(builder.data.NombreDePasDeTempsPourUneOptimisation - 1)
          .HydroLevel(pays, -1.0)
          .equalTo()
          .build();
    }
}
