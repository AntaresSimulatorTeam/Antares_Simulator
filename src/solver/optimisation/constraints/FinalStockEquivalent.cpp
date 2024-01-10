/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
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
