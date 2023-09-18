#include "BindingConstraintDayGroup.h"
#include "new_constraint_builder_utils.h"

std::shared_ptr<BindingConstraintDayData>
  BindingConstraintDayGroup::GetBindingConstraintDayDataFromProblemHebdo(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo_->MatriceDesContraintesCouplantes[cntCouplante];

    BindingConstraintDayData data
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
         problemeHebdo_->PaliersThermiquesDuPays,
         problemeHebdo_->CorrespondanceCntNativesCntOptimJournalieres,
         problemeHebdo_->NombreDePasDeTempsDUneJournee,
         problemeHebdo_->NumeroDeJourDuPasDeTemps};

    return std::make_shared<BindingConstraintDayData>(data);
}
void BindingConstraintDayGroup::Build()
{
    std::shared_ptr<NewConstraintBuilder> builder(
      NewGetConstraintBuilderFromProblemHebdo(problemeHebdo_));

    BindingConstraintDay bindingConstraintDay(builder);
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante,
                                 GetBindingConstraintDayDataFromProblemHebdo(cntCouplante));
    }
}