/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
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
#include "HydroPower.h"

void HydroPower::add(int pays)
{
    bool presenceHydro
      = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable;
    bool TurbEntreBornes = problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes;

    const int NombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;
    if (presenceHydro && !TurbEntreBornes)
    {
        if (bool presencePompage
            = problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable)
        {
            problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays]
              = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

            const double pumpingRatio
              = problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio;
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.HydProd(pays, 1.0).Pumping(pays, -pumpingRatio);
            }
        }
        else
        {
            for (int pdt = 0; pdt < NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                builder.updateHourWithinWeek(pdt);
                builder.HydProd(pays, 1.0);
            }
        }
        problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays]
          = problemeHebdo->ProblemeAResoudre->NombreDeContraintes;

        builder.equalTo();
        ConstraintNamer namer(problemeHebdo->ProblemeAResoudre->NomDesContraintes);
        namer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
        namer.UpdateTimeStep(problemeHebdo->weekInTheYear);
        namer.HydroPower(problemeHebdo->ProblemeAResoudre->NombreDeContraintes);
        builder.build();
    }
    else
        problemeHebdo->NumeroDeContrainteEnergieHydraulique[pays] = -1;
}
