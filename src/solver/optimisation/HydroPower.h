#pragma once
#include "constraint_builder.h"
struct HydroPowerData
{
    const bool& presenceHydro;
    const int& TurbEntreBornes;
    const bool& presencePompage;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteEnergieHydraulique;
    const double& pumpingRatio;
};

class HydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, HydroPowerData& data);
};
