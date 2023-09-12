#pragma once
#include "constraint_builder.h"
struct NbUnitsOutageLessThanNbUnitsStopData : public StartUpCostsData
{
    std::vector<int>& NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
};

class NbUnitsOutageLessThanNbUnitsStop : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pays, int cluster, int clusterIndex, int pdt, bool Simulation, NbUnitsOutageLessThanNbUnitsStopData&data);
    int nbTermesContraintesPourLesCoutsDeDemarrage = 0;
};