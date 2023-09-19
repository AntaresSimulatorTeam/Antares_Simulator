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

class AreaBalance : public NewConstraint
{
public:
    using NewConstraint::NewConstraint;

    void add(int pdt, int pays, std::shared_ptr<AreaBalanceData> data);
    void odd(int pays, std::shared_ptr<AreaBalanceData> data);
    // void add(int pdt, int pays, AreaBalanceData& data);
};