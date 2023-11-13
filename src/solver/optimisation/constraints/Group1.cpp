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

std::shared_ptr<ShortTermStorageLevelData> Group1::GetShortTermStorageLevelData(int pdt)
{
    ShortTermStorageLevelData shortTermStorageLevelData = {
      problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].ShortTermStorageLevelConstraint,
      problemeHebdo_->ShortTermStorage,
    };

    return std::make_shared<ShortTermStorageLevelData>(shortTermStorageLevelData);
}

std::shared_ptr<FlowDissociationData> Group1::GetFlowDissociationData(int pdt)
{
    FlowDissociationData flowDissociationData = {
      problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDeDissociationDeFlux,
      problemeHebdo_->CoutDeTransport,
      problemeHebdo_->PaysOrigineDeLInterconnexion,
      problemeHebdo_->PaysExtremiteDeLInterconnexion};

    return std::make_shared<FlowDissociationData>(flowDissociationData);
}

std::shared_ptr<BindingConstraintHourData> Group1::GetBindingConstraintHourData(int pdt,
                                                                                int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo_->MatriceDesContraintesCouplantes[cntCouplante];
    BindingConstraintData bindingConstraintData
      = {MatriceDesContraintesCouplantes.TypeDeContrainteCouplante,
         MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante,
         MatriceDesContraintesCouplantes.NumeroDeLInterconnexion,
         MatriceDesContraintesCouplantes.PoidsDeLInterconnexion,
         MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco,
         MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante,
         MatriceDesContraintesCouplantes.PaysDuPalierDispatch,
         MatriceDesContraintesCouplantes.NumeroDuPalierDispatch,
         MatriceDesContraintesCouplantes.PoidsDuPalierDispatch,
         MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch,
         MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante,
         MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante,
         problemeHebdo_->PaliersThermiquesDuPays};

    BindingConstraintHourData bindingConstraintHourData
      = {bindingConstraintData,
         problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
           .NumeroDeContrainteDesContraintesCouplantes};
    return std::make_shared<BindingConstraintHourData>(bindingConstraintHourData);
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

            shortTermStorageLevel.add(pdt, pays, GetShortTermStorageLevelData(pdt));
        }

        for (uint32_t interco = 0; interco < problemeHebdo_->NombreDInterconnexions; interco++)
        {
            flowDissociation.add(pdt, interco, GetFlowDissociationData(pdt));
        }

        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintHour.add(
              pdt, cntCouplante, GetBindingConstraintHourData(pdt, cntCouplante));
        }
    }
}
