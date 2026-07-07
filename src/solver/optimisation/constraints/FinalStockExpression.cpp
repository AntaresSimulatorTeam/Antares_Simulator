// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/FinalStockExpression.h"

void FinalStockExpression::add(int pays)
{
    const auto pdt = builder.data.NombreDePasDeTempsPourUneOptimisation - 1;

    if (data.CaracteristiquesHydrauliques[pays].AccurateWaterValue)
    /*  expression constraint : - StockFinal +sum (stocklayers) = 0*/
    {
        builder.updateHourWithinWeek(pdt).FinalStorage(pays, -1.0);
        for (int layerindex = 0; layerindex < 100; layerindex++)
        {
            builder.LayerStorage(pays, layerindex, 1.0);
        }
        data.NumeroDeContrainteExpressionStockFinal[pays] = builder.data.nombreDeContraintes;

        // The recording constructor: this dual feeds the `hydro_shadow_price`
        // extra output of the simulation table.
        ConstraintNamer namer(builder.data.NomDesContraintes, builder.data.LegacyConstraintsInfo);

        namer.UpdateArea(builder.data.NomsDesPays[pays]);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.FinalStockExpression(builder.data.nombreDeContraintes);
        builder.equalTo().build();
    }
}
