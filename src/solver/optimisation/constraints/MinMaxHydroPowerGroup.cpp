// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/MinMaxHydroPowerGroup.h"

MinHydroPowerData MinMaxHydroPowerGroup::GetMinHydroPowerData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                       ->NombreDePasDeTempsPourUneOptimisation,
            .NumeroDeContrainteMinEnergieHydraulique = problemeHebdo_
                                                         ->NumeroDeContrainteMinEnergieHydraulique};
}

MaxHydroPowerData MinMaxHydroPowerGroup::GetMaxHydroPowerData()
{
    return {.CaracteristiquesHydrauliques = problemeHebdo_->CaracteristiquesHydrauliques,
            .NombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                       ->NombreDePasDeTempsPourUneOptimisation,
            .NumeroDeContrainteMaxEnergieHydraulique = problemeHebdo_
                                                         ->NumeroDeContrainteMaxEnergieHydraulique};
}

void MinMaxHydroPowerGroup::BuildConstraints()
{
    auto minHydroPowerData = GetMinHydroPowerData();
    MinHydroPower minHydroPower(builder_, minHydroPowerData);
    auto maxHydroPowerData = GetMaxHydroPowerData();
    MaxHydroPower maxHydroPower(builder_, maxHydroPowerData);

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        minHydroPower.add(pays);
        maxHydroPower.add(pays);
    }
}
