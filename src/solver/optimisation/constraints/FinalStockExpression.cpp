#include "FinalStockExpression.h"

void FinalStockExpression::add(int pays, std::shared_ptr<FinalStockExpressionData> data)
{
    const auto pdt = builder->data->NombreDePasDeTempsPourUneOptimisation - 1;

    if (data->AccurateWaterValue)
    /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
    {
        builder->updateHourWithinWeek(pdt).FinalStorage(pays, -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder->LayerStorage(pays, layerindex, 1.0);
        }
        data->NumeroDeContrainteExpressionStockFinal[pays] = builder->data->nombreDeContraintes;

        ConstraintNamer namer(builder->data->NomDesContraintes);

        namer.UpdateArea(builder->data->NomsDesPays[pays]);
        namer.UpdateTimeStep(builder->data->weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(builder->data->nombreDeContraintes);
        builder->equalTo().build();
    }
}
