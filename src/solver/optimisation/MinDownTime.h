#pragma once
#include "constraint_builder.h"
struct MinDownTimeData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDArret;
};

class MinDownTime : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays,
             int cluster,
             int clusterIndex,
             int pdt,
             bool Simulation,
             MinDownTimeData& data);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};