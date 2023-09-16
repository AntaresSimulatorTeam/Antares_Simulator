#pragma once
#include "constraint_builder.h"
struct MaxPumpingData
{
    const bool& PresenceDePompageModulable;
    std::vector<int>& NumeroDeContrainteMaxPompage;
};

class MaxPumping : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, MaxPumpingData& data);
};