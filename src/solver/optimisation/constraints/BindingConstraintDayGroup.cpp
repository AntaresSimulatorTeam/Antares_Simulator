#include "BindingConstraintDayGroup.h"
#include "constraint_builder_utils.h"

BindingConstraintDayData BindingConstraintDayGroup::GetBindingConstraintDayDataFromProblemHebdo(
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
      /*.CorrespondanceCntNativesCntOptimJournalieres = */
      problemeHebdo_->CorrespondanceCntNativesCntOptimJournalieres,
      /*.NombreDePasDeTempsDUneJournee = */ problemeHebdo_->NombreDePasDeTempsDUneJournee,
      /*.NumeroDeJourDuPasDeTemps = */ problemeHebdo_->NumeroDeJourDuPasDeTemps};
}

void BindingConstraintDayGroup::Build()
{
    BindingConstraintDay bindingConstraintDay(builder_);
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        auto bindingConstraintDayData = GetBindingConstraintDayDataFromProblemHebdo(cntCouplante);
        bindingConstraintDay.add(cntCouplante, bindingConstraintDayData);
    }
}