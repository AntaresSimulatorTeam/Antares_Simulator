#include "antares/solver/optimisation/constraints/BindingConstraintDayGroup.h"
#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"

BindingConstraintDayData BindingConstraintDayGroup::GetBindingConstraintDayDataFromProblemHebdo()
{
    return {.MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
            .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
            .CorrespondanceCntNativesCntOptimJournalieres
            = problemeHebdo_->CorrespondanceCntNativesCntOptimJournalieres,
            .NombreDePasDeTempsDUneJournee = problemeHebdo_->NombreDePasDeTempsDUneJournee,
            .NumeroDeJourDuPasDeTemps = problemeHebdo_->NumeroDeJourDuPasDeTemps};
}

void BindingConstraintDayGroup::BuildConstraints()
{
    auto bindingConstraintDayData = GetBindingConstraintDayDataFromProblemHebdo();
    BindingConstraintDay bindingConstraintDay(builder_, bindingConstraintDayData);
    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }
}