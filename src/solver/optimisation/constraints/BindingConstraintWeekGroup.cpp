#include "BindingConstraintWeekGroup.h"
#include "constraint_builder_utils.h"

BindingConstraintWeekData BindingConstraintWeekGroup::GetBindingConstraintWeekDataFromProblemHebdo()
{
    return {.MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .NumeroDeContrainteDesContraintesCouplantes
            = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires
                .NumeroDeContrainteDesContraintesCouplantes};
}

void BindingConstraintWeekGroup::BuildConstraints()
{
    auto bindingConstraintWeekData = GetBindingConstraintWeekDataFromProblemHebdo();
    BindingConstraintWeek bindingConstraintWeek(builder_, bindingConstraintWeekData);

    if (problemeHebdo_->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo_->NombreDePasDeTempsDUneJournee)
    {
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }
}