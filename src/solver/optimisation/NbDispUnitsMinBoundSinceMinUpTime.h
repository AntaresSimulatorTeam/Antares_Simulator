#pragma once
#include "constraint_builder.h"
struct NbDispUnitsMinBoundSinceMinUpTimeData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
};

class NbDispUnitsMinBoundSinceMinUpTime : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays,
             int cluster,
             int clusterIndex,
             int pdt,
             bool Simulation,
             NbDispUnitsMinBoundSinceMinUpTimeData& data);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};