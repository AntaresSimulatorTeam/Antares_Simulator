#pragma once
#include "constraint_builder.h"

class PMinDispatchableGeneration : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation, StartUpCostsData& data);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};