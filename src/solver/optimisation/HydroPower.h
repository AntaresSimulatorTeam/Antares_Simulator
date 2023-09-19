#pragma once
#include "new_constraint_builder.h"
struct HydroPowerData
{
    const bool& presenceHydro;
    const int& TurbEntreBornes;
    const bool& presencePompage;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteEnergieHydraulique;
    const double& pumpingRatio;
};

class HydroPower : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, std::shared_ptr<HydroPowerData> data);
};
