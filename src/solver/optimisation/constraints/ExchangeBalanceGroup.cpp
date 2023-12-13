#include "ExchangeBalanceGroup.h"

ExchangeBalanceData ExchangeBalanceGroup::GetExchangeBalanceDataFromProblemHebdo()
{
    return {.IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
            .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
            .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
            .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
            .NumeroDeContrainteDeSoldeDEchange = problemeHebdo_->NumeroDeContrainteDeSoldeDEchange};
}

void ExchangeBalanceGroup::BuildConstraints()
{
    auto data = GetExchangeBalanceDataFromProblemHebdo();
    ExchangeBalance exchangeBalance(builder_, data);
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays - 1; pays++)
    {
        exchangeBalance.add(pays);
    }
}
