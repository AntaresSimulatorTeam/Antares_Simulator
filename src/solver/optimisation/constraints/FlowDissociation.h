#pragma once
#include "new_constraint_builder.h"

struct FlowDissociationData
{
    std::vector<int>& NumeroDeContrainteDeDissociationDeFlux;
    const std::vector<COUTS_DE_TRANSPORT>& CoutDeTransport;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};


/*!
 * represent 'Flow Dissociation' constraint type
 */

class FlowDissociation : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param interco : interconnection number
     */
    void add(int pdt, int interco, std::shared_ptr<FlowDissociationData> data);
};
