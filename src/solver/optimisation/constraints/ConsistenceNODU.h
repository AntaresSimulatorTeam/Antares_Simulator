#pragma once
#include "ConstraintBuilder.h"

class ConsistenceNODU : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};