#pragma once
#include "constraint_builder.h"

struct MinHydroPowerData
{
    const bool& presenceHydro;
    const bool& TurbEntreBornes;
    const bool& PresenceDePompageModulable;
    const int& NombreDePasDeTempsPourUneOptimisation;
    std::vector<int>& NumeroDeContrainteMinEnergieHydraulique;
};

class MinHydroPower : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, MinHydroPowerData& data);
};
