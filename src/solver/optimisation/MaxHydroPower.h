#pragma once
#include "new_constraint_builder.h"

struct MaxHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMaxEnergieHydraulique;
};
class MaxHydroPower : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, std::shared_ptr<MaxHydroPowerData> data);
};