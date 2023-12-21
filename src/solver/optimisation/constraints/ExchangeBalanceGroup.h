#pragma once
#include "ConstraintGroup.h"
#include "ExchangeBalance.h"

class ExchangeBalanceGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void BuildConstraints() override;

private:
    ExchangeBalanceData GetExchangeBalanceDataFromProblemHebdo();
};