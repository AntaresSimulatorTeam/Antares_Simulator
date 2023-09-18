#include "Group1.h"
#include "AreaBalance.h"
#include "FictitiousLoad.h"
#include "ShortTermStorageLevel.h"
#include "FlowDissociation.h"
#include "BindingConstraintHour.h"
#include "new_constraint_builder_utils.h"

/*TODO Rename this*/
void Group1::Build()
{
    auto ProblemeAResoudre = problemeHebdo_->ProblemeAResoudre.get();

    std::shared_ptr<NewConstraintBuilder> builder(
      NewGetConstraintBuilderFromProblemHebdo(problemeHebdo_));
    AreaBalance areaBalance(builder);

    FictitiousLoad fictitiousLoad(builder);
    ShortTermStorageLevel shortTermStorageLevel(builder);
    FlowDissociation flowDissociation(builder);
    BindingConstraintHour bindingConstraintHour(builder);

    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
          = problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            AreaBalanceData areaBalanceData{
              problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays,
              problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt]
                .SIM_ShortTermStorage.InjectionVariable,
              problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt]
                .SIM_ShortTermStorage.WithdrawalVariable,
              problemeHebdo_->IndexDebutIntercoOrigine,
              problemeHebdo_->IndexSuivantIntercoOrigine,
              problemeHebdo_->IndexDebutIntercoExtremite,
              problemeHebdo_->IndexSuivantIntercoExtremite,
              problemeHebdo_->PaliersThermiquesDuPays[pays],
              problemeHebdo_->ShortTermStorage};
            auto shared_ptr_areaBalance = std::make_shared<AreaBalanceData>(areaBalanceData);
            areaBalance.add(pdt, pays, shared_ptr_areaBalance);

            FictitiousLoadData fictitiousLoadData
              = {problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
                   .NumeroDeContraintePourEviterLesChargesFictives,
                 problemeHebdo_->PaliersThermiquesDuPays[pays],
                 problemeHebdo_->DefaillanceNegativeUtiliserHydro};

            auto shared_ptr_fictitiousLoad_data
              = std::make_shared<FictitiousLoadData>(fictitiousLoadData);
            fictitiousLoad.add(pdt, pays, shared_ptr_fictitiousLoad_data);

            ShortTermStorageLevelData shortTermStorageLevelData = {
              problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt].ShortTermStorageLevelConstraint,
              problemeHebdo_->ShortTermStorage,
            };
            auto shared_ptr_shortTermStorageLevelData
              = std::make_shared<ShortTermStorageLevelData>(shortTermStorageLevelData);

            shortTermStorageLevel.add(pdt, pays, shared_ptr_shortTermStorageLevelData);
        }

        for (uint32_t interco = 0; interco < problemeHebdo_->NombreDInterconnexions; interco++)
        {
            FlowDissociationData flowDissociationData
              = {problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt]
                   .NumeroDeContrainteDeDissociationDeFlux,
                 problemeHebdo_->CoutDeTransport,
                 problemeHebdo_->PaysOrigineDeLInterconnexion,
                 problemeHebdo_->PaysExtremiteDeLInterconnexion};
            auto shared_ptr_flowDissociationData
              = std::make_shared<FlowDissociationData>(flowDissociationData);
            flowDissociation.add(pdt, interco, shared_ptr_flowDissociationData);
        }
        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
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

            auto bindingConstraintHourData_shared
              = std::make_shared<BindingConstraintHourData>(bindingConstraintHourData);
            bindingConstraintHour.add(pdt, cntCouplante, bindingConstraintHourData_shared);
        }
    }
}
