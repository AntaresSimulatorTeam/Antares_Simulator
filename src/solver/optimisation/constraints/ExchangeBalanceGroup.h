#pragma once
#include "ConstraintGroup.h"
#include "ExchangeBalance.h"

class ExchangeBalanceGroup : public ConstraintGroup
{
public:
    using ConstraintGroup::ConstraintGroup;

    void buildConstraints() override;

private:
    ExchangeBalanceData GetExchangeBalanceDataFromProblemHebdo();
};