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

FlowDissociationData Group1::GetFlowDissociationData(int pdt)
{
    return {.NumeroDeContrainteDeDissociationDeFlux
            = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
                .NumeroDeContrainteDeDissociationDeFlux,
            .CoutDeTransport = problemeHebdo_->CoutDeTransport,
            .PaysOrigineDeLInterconnexion = problemeHebdo_->PaysOrigineDeLInterconnexion,
            .PaysExtremiteDeLInterconnexion = problemeHebdo_->PaysExtremiteDeLInterconnexion};
}

BindingConstraintHourData Group1::GetBindingConstraintHourData(int pdt, int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo_->MatriceDesContraintesCouplantes[cntCouplante];
    return

      {/* .TypeDeContrainteCouplante =*/MatriceDesContraintesCouplantes.TypeDeContrainteCouplante,
       /* .NombreDInterconnexionsDansLaContrainteCouplante=*/
       MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante,
       /* .NumeroDeLInterconnexion =*/MatriceDesContraintesCouplantes.NumeroDeLInterconnexion,
       /* .PoidsDeLInterconnexion =*/MatriceDesContraintesCouplantes.PoidsDeLInterconnexion,
       /* .OffsetTemporelSurLInterco =*/MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco,
       /* .NombreDePaliersDispatchDansLaContrainteCouplante=*/
       MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante,
       /* .PaysDuPalierDispatch =*/MatriceDesContraintesCouplantes.PaysDuPalierDispatch,
       /* .NumeroDuPalierDispatch =*/MatriceDesContraintesCouplantes.NumeroDuPalierDispatch,
       /* .PoidsDuPalierDispatch =*/MatriceDesContraintesCouplantes.PoidsDuPalierDispatch,
       /* .OffsetTemporelSurLePalierDispatch=*/
       MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch,
       /* .SensDeLaContrainteCouplante =*/
       MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante,
       /* .NomDeLaContrainteCouplante =*/MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante,
       /* .PaliersThermiquesDuPays =*/problemeHebdo_->PaliersThermiquesDuPays,
       /*.NumeroDeContrainteDesContraintesCouplantes=*/
       problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
         .NumeroDeContrainteDesContraintesCouplantes};
}

void Group1::Build()
{
    auto areaBalanceData = GetAreaBalanceData();
    AreaBalance areaBalance(builder_, areaBalanceData);

    auto fictitiousLoadData = GetFictitiousLoadData();
    FictitiousLoad fictitiousLoad(builder_, fictitiousLoadData);

    auto shortTermStorageLevelData = GetShortTermStorageLevelData();
    ShortTermStorageLevel shortTermStorageLevel(builder_, shortTermStorageLevelData);
    FlowDissociation flowDissociation(builder_);
    BindingConstraintHour bindingConstraintHour(builder_);

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
            auto flowDissociationData = GetFlowDissociationData(pdt);
            flowDissociation.add(pdt, interco, flowDissociationData);
        }

        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            auto bindingConstraintHourData = GetBindingConstraintHourData(pdt, cntCouplante);
            bindingConstraintHour.add(pdt, cntCouplante, bindingConstraintHourData);
        }
    }
}
