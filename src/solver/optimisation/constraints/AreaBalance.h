#pragma once
#include "new_constraint_builder.h"

struct AreaBalanceData
{
    std::vector<int>& NumeroDeContrainteDesBilansPays;
    std::vector<int>& InjectionVariable;
    std::vector<int>& WithdrawalVariable;
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;
    const PALIERS_THERMIQUES& PaliersThermiquesDuPays;
    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage;
};



/*!
 * represent 'Area Balance' constraint type
 */

class AreaBalance : public NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;

    /*!
     * @brief Add variables to the constraint and update constraints Matrix
     * @param pdt : timestep
     * @param pays : area
     */

    void add(int pdt, int pays, std::shared_ptr<AreaBalanceData> data);
};
