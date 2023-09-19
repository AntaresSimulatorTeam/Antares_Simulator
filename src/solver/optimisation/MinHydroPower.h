#pragma once
#include "new_constraint_builder.h"

struct MinHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMinEnergieHydraulique;
};

class MinHydroPower : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, std::shared_ptr<MinHydroPowerData> data);
};
