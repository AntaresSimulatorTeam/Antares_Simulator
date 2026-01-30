// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/constraints/Group1.h"

#include "antares/solver/optimisation/constraints/ShortTermStorageCostVariation.h"
#include "antares/solver/optimisation/constraints/ShortTermStorageCumulation.h"

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

MaxUnsupEnergyData Group1::GetMaxUnsupEnergyData()
{
    return {.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim};
}

ShortTermStorageData Group1::GetShortTermStorageData()
{
    return {
      .CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
      .ShortTermStorage = problemeHebdo_->ShortTermStorage,
    };
}

ShortTermStorageCumulativeConstraintData Group1::GetShortTermStorageCumulativeConstraintData()
{
    return {{.CorrespondanceCntNativesCntOptim = problemeHebdo_->CorrespondanceCntNativesCntOptim,
             .ShortTermStorage = problemeHebdo_->ShortTermStorage},
            problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires};
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

    auto maxUnsupEnergyData = GetMaxUnsupEnergyData();
    MaxUnsuppliedEnergy maxUnsuppliedEnergy(builder_, maxUnsupEnergyData);

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

    auto shortTermStorageCumulativeConstraintData = GetShortTermStorageCumulativeConstraintData();
    ShortTermStorageCumulation shortTermStorageCumulation(builder_,
                                                          shortTermStorageCumulativeConstraintData);

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
            maxUnsuppliedEnergy.add(pdt, pays);
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

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; ++pays)
    {
        shortTermStorageCumulation.add(pays);
    }
}
