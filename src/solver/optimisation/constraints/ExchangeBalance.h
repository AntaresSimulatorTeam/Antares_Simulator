
#pragma once
#include "new_constraint_builder.h"

struct ExchangeBalanceData
{
    const std::vector<int>& IndexDebutIntercoOrigine;
    const std::vector<int>& IndexSuivantIntercoOrigine;
    const std::vector<int>& IndexDebutIntercoExtremite;
    const std::vector<int>& IndexSuivantIntercoExtremite;
    std::vector<int>& NumeroDeContrainteDeSoldeDEchange;
};

class ExchangeBalance : private NewConstraintFactory
{
public:
    using NewConstraintFactory::NewConstraintFactory;
    void add(uint32_t pays, std::shared_ptr<ExchangeBalanceData> data);
};