#include "FinalStockExpression.h"

void FinalStockExpression::add(int pays, FinalStockExpressionData& data)
{
    const auto pdt = data.pdt;

    if (data.AccurateWaterValue)
    /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
    {
        builder.updateHourWithinWeek(pdt).include(Variable::FinalStorage(pays), -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder.include(Variable::LayerStorage(pays, layerindex), 1.0);
        }
        data.NumeroDeContrainteExpressionStockFinal[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes,
                              problemeHebdo->NamedProblems);

        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.equalTo().build();
    }
}
