#pragma once
#include "ConstraintGroup.h"
#include "ExchangeBalance.h"

class ExchangeBalanceGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void Build() override;

private:
    std::shared_ptr<ExchangeBalanceData> GetExchangeBalanceDataFromProblemHebdo();
};