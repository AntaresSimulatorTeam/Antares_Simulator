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

#include "antares/solver/optimisation/constraints/BindingConstraintDay.h"

void BindingConstraintDay::add(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = data.MatriceDesContraintesCouplantes[cntCouplante];
    if (MatriceDesContraintesCouplantes.TypeDeContrainteCouplante != CONTRAINTE_JOURNALIERE)
    {
        return;
    }

    const int nbInterco = MatriceDesContraintesCouplantes
                            .NombreDInterconnexionsDansLaContrainteCouplante;
    const int nbClusters = MatriceDesContraintesCouplantes
                             .NombreDePaliersDispatchDansLaContrainteCouplante;

    const int NombreDePasDeTempsPourUneOptimisation = builder.data
                                                        .NombreDePasDeTempsPourUneOptimisation;
    const int NombreDePasDeTempsDUneJournee = data.NombreDePasDeTempsDUneJournee;
    int pdtDebut = 0;
    while (pdtDebut < NombreDePasDeTempsPourUneOptimisation)
    {
        int jour = data.NumeroDeJourDuPasDeTemps[pdtDebut];

        for (int index = 0; index < nbInterco; index++)
        {
            int interco = MatriceDesContraintesCouplantes.NumeroDeLInterconnexion[index];
            double poids = MatriceDesContraintesCouplantes.PoidsDeLInterconnexion[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                builder.updateHourWithinWeek(pdt).NTCDirect(interco,
                                                            poids,
                                                            offset,
                                                            builder.data.NombreDePasDeTemps);
            }
        }

        for (int index = 0; index < nbClusters; index++)
        {
            int pays = MatriceDesContraintesCouplantes.PaysDuPalierDispatch[index];
            const PALIERS_THERMIQUES& PaliersThermiquesDuPays = data.PaliersThermiquesDuPays[pays];
            const int palier = PaliersThermiquesDuPays
                                 .NumeroDuPalierDansLEnsembleDesPaliersThermiques
                                   [MatriceDesContraintesCouplantes.NumeroDuPalierDispatch[index]];
            double poids = MatriceDesContraintesCouplantes.PoidsDuPalierDispatch[index];
            int offset = MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch[index];

            for (int pdt = pdtDebut; pdt < pdtDebut + NombreDePasDeTempsDUneJournee; pdt++)
            {
                builder.updateHourWithinWeek(pdt)
                  .DispatchableProduction(palier, poids, offset, builder.data.NombreDePasDeTemps);
            }
        }

        data.CorrespondanceCntNativesCntOptimJournalieres[jour]
          .NumeroDeContrainteDesContraintesCouplantes[cntCouplante]
          = builder.data.nombreDeContraintes;

        builder.SetOperator(MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante);
        {
            ConstraintNamer namer(builder.data.NomDesContraintes);
            namer.UpdateTimeStep(jour);
            namer.BindingConstraintDay(builder.data.nombreDeContraintes,
                                       MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante);
        }
        builder.build();
        pdtDebut += data.NombreDePasDeTempsDUneJournee;
    }
}
