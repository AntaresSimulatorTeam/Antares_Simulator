#include "FinalStockEquivalent.h"

void FinalStockEquivalent::add(int pays, FinalStockEquivalentData& data)
{
    const auto pdt = data.pdt;
    if (data.AccurateWaterValue && data.DirectLevelAccess)
    { /*  equivalence constraint : StockFinal- Niveau[T]= 0*/

        data.NumeroDeContrainteEquivalenceStockFinal[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockEquivalent(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

    builder.updateHourWithinWeek(pdt)
      .include(Variable::FinalStorage(pays), 1.0)
      .updateHourWithinWeek(problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1)
      .include(Variable::HydroLevel(pays), -1.0)
      .equalTo()
      .build();
    }
}
