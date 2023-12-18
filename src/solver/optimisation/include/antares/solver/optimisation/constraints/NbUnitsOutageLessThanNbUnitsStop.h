#pragma once
#include "ConstraintBuilder.h"
struct NbUnitsOutageLessThanNbUnitsStopData
{
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    bool Simulation;
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
};
/*!
 * represent 'NbUnitsOutageLessThanNbUnitsStop' type
 */
class NbUnitsOutageLessThanNbUnitsStop : private ConstraintFactory
{
public:
    NbUnitsOutageLessThanNbUnitsStop(ConstraintBuilder& builder,
                                     NbUnitsOutageLessThanNbUnitsStopData& data) :
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
    NbUnitsOutageLessThanNbUnitsStopData& data;
};