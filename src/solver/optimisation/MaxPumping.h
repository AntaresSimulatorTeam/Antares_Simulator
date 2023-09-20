#pragma once
#include "new_constraint_builder.h"
struct MaxPumpingData
{
    const bool& PresenceDePompageModulable;
    std::vector<int>& NumeroDeContrainteMaxPompage;
};

class MaxPumping : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pays, std::shared_ptr<MaxPumpingData> data);
};