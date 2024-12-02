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

#include "antares/solver/optimisation/constraints/Group1.h"

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"

AreaBalanceData Group1::GetAreaBalanceData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
            .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
            .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
            .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .ShortTermStorage = problemeHebdo_->ShortTermStorage};
}

FictitiousLoadData Group1::GetFictitiousLoadData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .DefaillanceNegativeUtiliserHydro = problemeHebdo_->DefaillanceNegativeUtiliserHydro};
}

ShortTermStorageData Group1::GetShortTermStorageData()
{
    return {
      .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
      .ShortTermStorage = problemeHebdo_->ShortTermStorage,
    };
}

FlowDissociationData Group1::GetFlowDissociationData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
            .CoutDeTransport = problemeHebdo_->CoutDeTransport,
            .PaysOrigineDeLInterconnexion = problemeHebdo_->PaysOrigineDeLInterconnexion,
            .PaysExtremiteDeLInterconnexion = problemeHebdo_->PaysExtremiteDeLInterconnexion};
}

BindingConstraintHourData Group1::GetBindingConstraintHourData()
{
    return

      {.MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
       .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
       .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays};
}

void Group1::BuildConstraints()
{
    auto areaBalanceData = GetAreaBalanceData();
    AreaBalance areaBalance(builder_, areaBalanceData);

    auto fictitiousLoadData = GetFictitiousLoadData();
    FictitiousLoad fictitiousLoad(builder_, fictitiousLoadData);

    auto shortTermStorageData = GetShortTermStorageData();
    ShortTermStorageLevel shortTermStorageLevel(builder_, shortTermStorageData);

    ShortTermStorageCostVariationInjectionBackward shortTermStorageCostVariationInjectionBackward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationInjectionForward shortTermStorageCostVariationInjectionForward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationWithdrawalBackward shortTermStorageCostVariationWithdrawalBackward(
      builder_,
      shortTermStorageData);
    ShortTermStorageCostVariationWithdrawalForward shortTermStorageCostVariationWithdrawalForward(
      builder_,
      shortTermStorageData);
    auto flowDissociationData = GetFlowDissociationData();
    FlowDissociation flowDissociation(builder_, flowDissociationData);

    auto bindingConstraintHourData = GetBindingConstraintHourData();
    BindingConstraintHour bindingConstraintHour(builder_, bindingConstraintHourData);

    int nombreDePasDeTempsPourUneOptimisation = problemeHebdo_
                                                  ->NombreDePasDeTempsPourUneOptimisation;

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            areaBalance.add(pdt, pays);
            fictitiousLoad.add(pdt, pays);
            shortTermStorageLevel.add(pdt, pays);
            shortTermStorageCostVariationInjectionBackward.add(pdt, pays);
            shortTermStorageCostVariationInjectionForward.add(pdt, pays);
            shortTermStorageCostVariationWithdrawalBackward.add(pdt, pays);
            shortTermStorageCostVariationWithdrawalForward.add(pdt, pays);
        }

        for (uint32_t interco = 0; interco < problemeHebdo_->NombreDInterconnexions; interco++)
        {
            flowDissociation.add(pdt, interco);
        }

        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintHour.add(pdt, cntCouplante);
        }
    }
}
