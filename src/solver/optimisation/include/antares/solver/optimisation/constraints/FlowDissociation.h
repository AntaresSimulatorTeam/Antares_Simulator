// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "ConstraintBuilder.h"

struct FlowDissociationData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<COUTS_DE_TRANSPORT>& CoutDeTransport;
    const std::vector<int>& PaysOrigineDeLInterconnexion;
    const std::vector<int>& PaysExtremiteDeLInterconnexion;
};

/*!
 * represent 'Flow Dissociation' constraint type
 */

class FlowDissociation final: private ConstraintFactory
{
public:
    FlowDissociation(ConstraintBuilder& builder, FlowDissociationData& data):
        ConstraintFactory(builder),
        data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param interco : interconnection number
     */
    void add(int pdt, int interco);

private:
    FlowDissociationData& data;
};
