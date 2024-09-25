/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include "antares/solver/optimisation/constraints/BindingConstraintHour.h"

void BindingConstraintHour::add(int pdt, int cntCouplante)
{
    data.CorrespondanceCntNativesCntOptim[pdt]
      .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
      = builder.data.nombreDeContraintes;

    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[cntCouplante];

    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_HORAIRE)
    {
        return;
    }

    builder.updateHourWithinWeek(pdt);
    // Links
    const int nbInterco = MatriceDesContraintesCouplantes
                            .NombreDInterconnexionsDansLaContrainteCouplante;
    for (int index = 0; index < nbInterco; index++)
    {
        const int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
        const double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];
        builder.updateHourWithinWeek(pdt).NTCDirect(interco,
                                                    poids,
                                                    offset,
                                                    builder.data.NombreDePasDeTemps);
    }

    // Thermal clusters
    const int nbClusters = MatriceDesContraintesCouplantes
                             .NombreDePaliersDispatchDansLaContrainteCouplante;
    for (int index = 0; index < nbClusters; index++)
    {
        const int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
        const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
        const int palier = PaliersThermiquesDuPays.NumeroDuPalierDansLEnsembleDesPaliersThermiques
                             [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
        const double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
        const int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

        builder.updateHourWithinWeek(pdt).DispatchableProduction(palier,
                                                                 poids,
                                                                 offset,
                                                                 builder.data.NombreDePasDeTemps);
    }

    builder.SetOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
    {
        ConstraintNamer namer(builder.data.NomDesContraintes);
        namer.UpdateTimeStep(builder.data.weekInTheYear * 168 + pdt);
        namer.BindingConstraintHour(builder.data.nombreDeContraintes,
                                    MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
    }
    builder.build();
}
