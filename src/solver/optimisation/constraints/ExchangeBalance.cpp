#include "ExchangeBalance.h"

void ExchangeBalance::add(uint32_t pays)
{
    builder.updateHourWithinWeek(0);

    int interco = data.IndexDebutIntercoOrigine[pays];
    while (interco >= 0)
    {
        builder.NTCDirect(interco, 1.0);
        interco = data.IndexSuivantIntercoOrigine[interco];
    }
    interco = data.IndexDebutIntercoExtremite[pays];
    while (interco >= 0)
    {
        builder.NTCDirect(interco, -1.0);

        interco = data.IndexSuivantIntercoExtremite[interco];
    }

    data.NumeroDeContrainteDeSoldeDEchange[pays] = builder.data.nombreDeContraintes;
    builder.equalTo();
    builder.build();
}