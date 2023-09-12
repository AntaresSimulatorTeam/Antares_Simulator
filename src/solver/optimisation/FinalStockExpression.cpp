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
        data.NumeroDeContrainteExpressionStockFinal[pays] = builder.data.nombreDeContraintes;

        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.NamedProblems);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(builder.data.nombreDeContraintes);
        builder.equalTo().build();
    }
}
