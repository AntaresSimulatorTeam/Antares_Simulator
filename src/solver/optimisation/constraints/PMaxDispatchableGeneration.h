#pragma once
#include "new_constraint_builder.h"

struct PMaxDispatchableGenerationData
{
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    int cluster;
    int clusterIndex;
    int pdt;
    bool Simulation;
};

/*!
 * represent 'PMaxDispatchableGeneration' Constraint type
 */
class PMaxDispatchableGeneration : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, std::shared_ptr<PMaxDispatchableGenerationData> data);
};