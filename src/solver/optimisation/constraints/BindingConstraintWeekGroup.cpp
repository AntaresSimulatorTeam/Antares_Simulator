#include "BindingConstraintWeekGroup.h"
#include "constraint_builder_utils.h"

BindingConstraintWeekData BindingConstraintWeekGroup::GetBindingConstraintWeekDataFromProblemHebdo(
  int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo_->MatriceDesContraintesCouplantes[cntCouplante];

    return {
      /* .TypeDeContrainteCouplante =*/MatriceDesContraintesCouplantes.TypeDeContrainteCouplante,
      /* .NombreDInterconnexionsDansLaContrainteCouplante=*/
      MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante,
      /* .NumeroDeLInterconnexion =*/MatriceDesContraintesCouplantes.NumeroDeLInterconnexion,
      /* .PoidsDeLInterconnexion =*/MatriceDesContraintesCouplantes.PoidsDeLInterconnexion,
      /* .OffsetTemporelSurLInterco =*/
      MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco,
      /* .NombreDePaliersDispatchDansLaContrainteCouplante=*/
      MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante,
      /* .PaysDuPalierDispatch =*/MatriceDesContraintesCouplantes.PaysDuPalierDispatch,
      /* .NumeroDuPalierDispatch =*/MatriceDesContraintesCouplantes.NumeroDuPalierDispatch,
      /* .PoidsDuPalierDispatch =*/MatriceDesContraintesCouplantes.PoidsDuPalierDispatch,
      /* .OffsetTemporelSurLePalierDispatch=*/
      MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch,
      /* .SensDeLaContrainteCouplante =*/
      MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante,
      /* .NomDeLaContrainteCouplante =*/
      MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante,
      /* .PaliersThermiquesDuPays =*/problemeHebdo_->PaliersThermiquesDuPays,
      /* .NumeroDeContrainteDesContraintesCouplantes =*/
      problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires
        .NumeroDeContrainteDesContraintesCouplantes};
}

void BindingConstraintWeekGroup::Build()
{
    BindingConstraintWeek bindingConstraintWeek(builder_);

    if (problemeHebdo_->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo_->NombreDePasDeTempsDUneJournee)
    {
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            auto bindingConstraintWeekData
              = GetBindingConstraintWeekDataFromProblemHebdo(cntCouplante);
            bindingConstraintWeek.add(cntCouplante, bindingConstraintWeekData);
        }
    }
}