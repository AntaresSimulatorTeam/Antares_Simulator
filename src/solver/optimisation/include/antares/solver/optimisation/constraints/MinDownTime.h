#pragma once
#include "ConstraintBuilder.h"

struct MinDownTimeData
{
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    bool Simulation;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
};

/*!
 * represent 'MinDownTime' Constraint type
 */
class MinDownTime : private ConstraintFactory
{
public:
    MinDownTime(ConstraintBuilder& builder, MinDownTimeData& data) :
     ConstraintFactory(builder), data(data)
    {
    }
    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pays : area
     * @param cluster : global index of the cluster
     * @param pdt : timestep
     * @param Simulation : ---
     */
    void add(int pays, int index, int pdt);

private:
    MinDownTimeData& data;
};