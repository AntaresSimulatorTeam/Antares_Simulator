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

void BindingConstraintWeekGroup::Build()
{
    auto bindingConstraintWeekData = GetBindingConstraintWeekDataFromProblemHebdo();
    BindingConstraintWeek bindingConstraintWeek(builder_, bindingConstraintWeekData);

    if (builder_.data.NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo_->NombreDePasDeTempsDUneJournee)
    {
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0; cntCouplante < builder_.data.NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }
}