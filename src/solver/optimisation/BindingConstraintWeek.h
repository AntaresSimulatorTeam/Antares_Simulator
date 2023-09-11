#pragma once
#include "constraint_builder.h"

struct BindingConstraintWeekData : public BindingConstraintData
{
    // std::vector<std::vector<int>&>& CorrespondanceCntNativesCntOptimJournalieres;
    std::vector<int>& NumeroDeContrainteDesContraintesCouplantes;
    // CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires;
};

class BindingConstraintWeek : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int cntCouplante, BindingConstraintWeekData& data);
};