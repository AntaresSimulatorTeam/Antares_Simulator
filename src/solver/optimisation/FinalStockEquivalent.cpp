#include "FinalStockEquivalent.h"

void FinalStockEquivalent::add(int pays, FinalStockEquivalentData& data)
{
    const auto pdt = data.pdt;
    if (data.AccurateWaterValue && data.DirectLevelAccess)
    { /*  equivalence constraint : StockFinal- Niveau[T]= 0*/

        data.NumeroDeContrainteEquivalenceStockFinal[pays] = builder.data.nombreDeContraintes;
        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FinalStockEquivalent(builder.data.nombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .include(Variable::FinalStorage(pays), 1.0)
          .updateHourWithinWeek(builder.data.NombreDePasDeTempsPourUneOptimisation - 1)
          .include(Variable::HydroLevel(pays), -1.0)
          .equalTo()
          .build();
    }
}
