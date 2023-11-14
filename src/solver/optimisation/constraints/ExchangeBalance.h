
#pragma once
#include "ConstraintBuilder.h"

struct ExchangeBalanceData
{
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;
    std::vector<int>& NumeroDeContrainteDeSoldeDEchange;
};

class ExchangeBalance : private ConstraintFactory
{
public:
    ExchangeBalance(ConstraintBuilder& builder, ExchangeBalanceData& data) :
     ConstraintFactory(builder), data(data)
    {
    }

    // TODO transform data to ref
    void add(uint32_t pays);

private:
    ExchangeBalanceData& data;
};