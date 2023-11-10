#include "ExchangeBalanceGroup.h"

std::shared_ptr<ExchangeBalanceData> ExchangeBalanceGroup::GetExchangeBalanceDataFromProblemHebdo()
{
    ExchangeBalanceData data
      = {.IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
         .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
         .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
         .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
         .NumeroDeContrainteDeSoldeDEchange = problemeHebdo_->NumeroDeContrainteDeSoldeDEchange};

    return std::make_shared<ExchangeBalanceData>(data);
}

void ExchangeBalanceGroup::Build()
{
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays - 1; pays++)
    {
        ExchangeBalance exchangeBalance(builder_);
        exchangeBalance.add(pays, GetExchangeBalanceDataFromProblemHebdo());
    }
}
