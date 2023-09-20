#pragma once
#include "new_constraint_builder.h"

struct AreaHydroLevelData
{
    std::vector<int>& NumeroDeContrainteDesNiveauxPays;
    const bool& SuiviNiveauHoraire;
    const double& PumpingRatio;
};

class AreaHydroLevel : private NewConstraint
{
    public:
        using NewConstraint::NewConstraint;
        void add(int pays, int pdt, std::shared_ptr<AreaHydroLevelData> data);
};