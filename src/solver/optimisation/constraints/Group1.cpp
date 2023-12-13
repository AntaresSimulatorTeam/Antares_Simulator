#include "Group1.h"
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

ShortTermStorageLevelData Group1::GetShortTermStorageLevelData()
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

    auto shortTermStorageLevelData = GetShortTermStorageLevelData();
    ShortTermStorageLevel shortTermStorageLevel(builder_, shortTermStorageLevelData);

    auto flowDissociationData = GetFlowDissociationData();
    FlowDissociation flowDissociation(builder_, flowDissociationData);

    auto bindingConstraintHourData = GetBindingConstraintHourData();
    BindingConstraintHour bindingConstraintHour(builder_, bindingConstraintHourData);

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            areaBalance.add(pdt, pays);
            fictitiousLoad.add(pdt, pays);
            shortTermStorageLevel.add(pdt, pays);
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
