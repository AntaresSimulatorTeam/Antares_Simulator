#include "Group1.h"
AreaBalanceData Group1::GetAreaBalanceData(int pdt, uint32_t pays)
{
    return {.NumeroDeContrainteDesBilansPays
            = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays,
            .InjectionVariable = problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt]
                                   .SIM_ShortTermStorage.InjectionVariable,
            .WithdrawalVariable = problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt]
                                    .SIM_ShortTermStorage.WithdrawalVariable,
            .IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
            .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
            .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
            .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays[pays],
            .ShortTermStorage = problemeHebdo_->ShortTermStorage};
}

FictitiousLoadData Group1::GetFictitiousLoadData(int pdt, uint32_t pays)
{
    return {.NumeroDeContraintePourEviterLesChargesFictives
            = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
                .NumeroDeContraintePourEviterLesChargesFictives,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays[pays],
            .DefaillanceNegativeUtiliserHydro = problemeHebdo_->DefaillanceNegativeUtiliserHydro};
}

ShortTermStorageLevelData Group1::GetShortTermStorageLevelData(int pdt)
{
    return {
      .ShortTermStorageLevelConstraint
      = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].ShortTermStorageLevelConstraint,
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
    AreaBalance areaBalance(builder_);
    FictitiousLoad fictitiousLoad(builder_);
    ShortTermStorageLevel shortTermStorageLevel(builder_);
    FlowDissociation flowDissociation(builder_);
    BindingConstraintHour bindingConstraintHour(builder_);

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            auto areaBalanceData = GetAreaBalanceData(pdt, pays);
            areaBalance.add(pdt, pays, areaBalanceData);

            auto fictitiousLoadData = GetFictitiousLoadData(pdt, pays);
            fictitiousLoad.add(pdt, pays, fictitiousLoadData);
            auto shortTermStorageLevelData = GetShortTermStorageLevelData(pdt);
            shortTermStorageLevel.add(pdt, pays, shortTermStorageLevelData);
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
