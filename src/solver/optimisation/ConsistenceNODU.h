#pragma once
#include "constraint_builder.h"

class ConsistenceNODU : public Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};