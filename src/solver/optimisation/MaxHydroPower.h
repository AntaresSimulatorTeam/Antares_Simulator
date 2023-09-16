#pragma once
#include "constraint_builder.h"

struct MaxHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMaxEnergieHydraulique;
};
class MaxHydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, MaxHydroPowerData& data);
};