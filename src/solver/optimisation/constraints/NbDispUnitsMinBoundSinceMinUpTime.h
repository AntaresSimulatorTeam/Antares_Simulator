#pragma once
#include "ConstraintBuilder.h"

class NbDispUnitsMinBoundSinceMinUpTime : private ConstraintFactory
{
public:
    using ConstraintFactory::ConstraintFactory;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};