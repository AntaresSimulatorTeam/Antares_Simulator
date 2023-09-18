#include "FinalStockExpression.h"

void FinalStockExpression::add(int pays)
{
    const auto pdt = problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1;

    if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
    /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
    {
        builder.updateHourWithinWeek(pdt).FinalStorage(pays, -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder.include(Variable::LayerStorage(pays, layerindex), 1.0);
        }
        problemeHebdo->NumeroDeContrainteExpressionStockFinal[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.equalTo().build();
    }
}
