#pragma once
#include "constraint_builder.h"

struct FlowDissociationData
{
    std::vector<int>& NumeroDeContrainteDeDissociationDeFlux;
    const std::vector<COUTS_DE_TRANSPORT>& CoutDeTransport;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};

class FlowDissociation : private Constraint
{
public:
    using Constraint::Constraint;
    void add(int pdt, int interco, FlowDissociationData& data);
};
