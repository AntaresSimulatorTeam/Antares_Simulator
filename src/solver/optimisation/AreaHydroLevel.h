#pragma once
#include "constraint_builder.h"

struct AreaHydroLevelData
{
    std::vector<int>& NumeroDeContrainteDesNiveauxPays;
    const bool& SuiviNiveauHoraire;
    const double& PumpingRatio;
};

class AreaHydroLevel : private Constraint
{
    public:
    using Constraint::Constraint;
    void add(int pays, int pdt, AreaHydroLevelData& data);
};