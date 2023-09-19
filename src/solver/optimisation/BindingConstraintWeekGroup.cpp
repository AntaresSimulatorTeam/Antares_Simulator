#include "BindingConstraintWeekGroup.h"
#include "new_constraint_builder_utils.h"

std::shared_ptr<BindingConstraintWeekData>
  BindingConstraintWeekGroup::GetBindingConstraintWeekDataFromProblemHebdo(int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo_->MatriceDesContraintesCouplantes[cntCouplante];

    BindingConstraintWeekData data
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
         problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires
           .NumeroDeContrainteDesContraintesCouplantes};

    return std::make_shared<BindingConstraintWeekData>(data);
}

void BindingConstraintWeekGroup::Build()
{
    std::shared_ptr<NewConstraintBuilder> builder(
      NewGetConstraintBuilderFromProblemHebdo(problemeHebdo_));
    BindingConstraintWeek bindingConstraintWeek(builder);

    if (problemeHebdo_->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo_->NombreDePasDeTempsDUneJournee)
    {
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante,
                                      GetBindingConstraintWeekDataFromProblemHebdo(cntCouplante));
        }
    }
}