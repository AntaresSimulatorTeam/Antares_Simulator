#pragma once
#include "new_constraint_builder.h"

struct BindingConstraintDayData : public BindingConstraintData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>&
      CorrespondanceCntNativesCntOptimJournalieres;

    const int32_t& NombreDePasDeTempsDUneJournee;

    std::vector<int32_t>& NumeroDeJourDuPasDeTemps;
};

class BindingConstraintDay : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int cntCouplante, std::shared_ptr<BindingConstraintDayData> data);
};
