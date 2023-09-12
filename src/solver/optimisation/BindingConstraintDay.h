#pragma once
#include "constraint_builder.h"

struct BindingConstraintDayData : public BindingConstraintData
{
    // std::vector<std::vector<int>&>& CorrespondanceCntNativesCntOptimJournalieres;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES>&
      CorrespondanceCntNativesCntOptimJournalieres;

    const int32_t NombreDePasDeTempsDUneJournee;

    std::vector<int32_t>& NumeroDeJourDuPasDeTemps;
};
class BindingConstraintDay : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante, BindingConstraintDayData& data);
};
