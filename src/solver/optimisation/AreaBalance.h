#pragma once
#include "constraint_builder.h"

struct AreaBalanceData
{
    AreaBalanceData(std::vector<int>& NumeroDeContrainteDesBilansPays,
                    std::vector<int>& InjectionVariable,
                    std::vector<int>& WithdrawalVariable,
                    const std::vector<int>& IndexDebutIntercoOrigine,
                    const std::vector<int>& IndexSuivantIntercoOrigine,
                    const std::vector<int>& IndexDebutIntercoExtremite,
                    const std::vector<int>& IndexSuivantIntercoExtremite,
                    const PALIERS_THERMIQUES& PaliersThermiquesDuPays,
                    const std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorage)

     :
     NumeroDeContrainteDesBilansPays(NumeroDeContrainteDesBilansPays),
     InjectionVariable(InjectionVariable),
     WithdrawalVariable(WithdrawalVariable),
     IndexDebutIntercoOrigine(IndexDebutIntercoOrigine),
     IndexSuivantIntercoOrigine(IndexSuivantIntercoOrigine),
     IndexDebutIntercoExtremite(IndexDebutIntercoExtremite),
     IndexSuivantIntercoExtremite(IndexSuivantIntercoExtremite),
     PaliersThermiquesDuPays(PaliersThermiquesDuPays),
     ShortTermStorage(ShortTermStorage)
    {
    }

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

class AreaBalance : private Constraint
{
public:
    using Constraint::Constraint;

    void add(int pdt, int pays, AreaBalanceData& data);
};