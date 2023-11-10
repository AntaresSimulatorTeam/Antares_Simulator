#pragma once
#include "ConstraintGroup.h"
#include "ExchangeBalance.h"

class ExchangeBalanceGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void Build() override;

private:
    std::shared_ptr<ExchangeBalanceData> GetExchangeBalanceDataFromProblemHebdo()
    {
        ExchangeBalanceData data
        {
            const std::vector<int>& IndexDebutIntercoOrigine;
            const std::vector<int>& IndexSuivantIntercoOrigine;
            const std::vector<int>& IndexDebutIntercoExtremite;
            const std::vector<int>& IndexSuivantIntercoExtremite;
            std::vector<int>& NumeroDeContrainteDeSoldeDEchange;
        };

        return std::make_shared<ExchangeBalanceData>(data);
    }
};