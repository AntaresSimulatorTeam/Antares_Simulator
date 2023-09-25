#include "FinalStockEquivalent.h"

void FinalStockEquivalent::add(int pays)
{
    const auto pdt = problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1;
    if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue
        && problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess)
    { /*  equivalence constraint : StockFinal- Niveau[T]= 0*/

        problemeHebdo->NumeroDeContrainteEquivalenceStockFinal[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockEquivalent(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);

        builder.updateHourWithinWeek(pdt)
          .FinalStorage(pays, 1.0)
          .updateHourWithinWeek(problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1)
          .HydroLevel(pays, -1.0)
          .equalTo()
          .build();
    }
}
