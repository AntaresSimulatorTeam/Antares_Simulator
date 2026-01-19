// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//! Precision sur la factorisation des matrices
#define ANTARES_CHOLESKY_EPSIMIN ((T)1.0e-9)

#include <cmath>

namespace Antares::Solver
{
template<class T, class U1, class U2>
bool Cholesky(U1& L, U2& A, uint size, T* temp)
{
    using namespace Yuni;

    for (uint i = 0; i != size; ++i)
    {
        temp[i] = 0;
    }

    T som;

    for (uint i = 0; i < size; ++i)
    {
        auto& Li = L[i];

        // on calcule d'abord L[i][i]
        som = A[i][i];
        for (int j = 0; j <= (int)(i - 1); ++j)
        {
            som -= Li[j] * Li[j];
        }

        if (som > ANTARES_CHOLESKY_EPSIMIN)
        {
            Li[i] = std::sqrt(som);

            // maintenant on cherche L[k][i], k > i.
            for (uint k = i + 1; k < size; ++k)
            {
                auto& Lk = L[k];
                auto& Ak = A[k];

                if (temp[k] == Ak[k])
                {
                    Lk[i] = 0;
                }
                else
                {
                    som = Ak[i];
                    for (int j = 0; j <= (int)(i - 1); ++j)
                    {
                        som -= Li[j] * Lk[j];
                    }

                    Lk[i] = som / Li[i];
                    temp[k] += Lk[i] * Lk[i];

                    // si temp[k] = A[k][k] la matrice n'est pas dp mais il est encore possible
                    // qu'elle soit sdp.
                    // si temp > A[k][k] alors il est certain que A n'est ni sdp ni dp donc on
                    // arrete le calcul
                    if (temp[k] > Ak[k])
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            // annule le reste de la colonne
            for (uint k = i; k != size; ++k)
            {
                L[k][i] = 0;
            }
        }
    }

    return false;
}

} // namespace Antares::Solver

#undef ANTARES_CHOLESKY_EPSIMIN
