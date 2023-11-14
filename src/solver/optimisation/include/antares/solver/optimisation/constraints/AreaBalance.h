#pragma once
#include "ConstraintBuilder.h"

struct AreaBalanceData
{
    std::vector<CORRESPONDANCES_DES_CONTRAINTES>& CorrespondanceCntNativesCntOptim;
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;
    const std::vector<PALIERS_THERMIQUES>& PaliersThermiquesDuPays;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};

/*!
 * represent 'Area Balance' constraint type
 */

class AreaBalance : public ConstraintFactory
{
public:
    AreaBalance(ConstraintBuilder& builder, AreaBalanceData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */

    void add(int pdt, int pays);

private:
    AreaBalanceData& data;
};
