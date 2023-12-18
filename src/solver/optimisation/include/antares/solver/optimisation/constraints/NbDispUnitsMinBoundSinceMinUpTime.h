#pragma once
#include "ConstraintBuilder.h"
struct NbDispUnitsMinBoundSinceMinUpTimeData
{
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    bool Simulation;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
};
/*!
 * represent 'Number of Dispatchable Units Min Bound Since Min Up Time' type
 */
class NbDispUnitsMinBoundSinceMinUpTime : private ConstraintFactory
{
public:
    NbDispUnitsMinBoundSinceMinUpTime(ConstraintBuilder& builder,
                                      NbDispUnitsMinBoundSinceMinUpTimeData& data) :
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
    NbDispUnitsMinBoundSinceMinUpTimeData& data;
};