#pragma once
#include "new_constraint_builder.h"

struct FlowDissociationData
{
    std::vector<int>& NumeroDeContrainteDeDissociationDeFlux;
    const std::vector<COUTS_DE_TRANSPORT>& CoutDeTransport;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};

class FlowDissociation : private NewConstraint
{
public:
    using NewConstraint::NewConstraint;
    void add(int pdt, int interco, std::shared_ptr<FlowDissociationData> data);
};
