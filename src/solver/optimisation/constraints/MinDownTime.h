#pragma once
#include "ConstraintBuilder.h"

class MinDownTime : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};