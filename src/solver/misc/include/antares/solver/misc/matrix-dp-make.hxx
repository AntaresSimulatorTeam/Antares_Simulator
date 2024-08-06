/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_HXX__
#define __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_HXX__

namespace Antares
{
namespace Solver
{
template<class T, class U1, class U2, class U3, class U4>
T MatrixDPMake(U1& L, U2& A, U3& B, U4& C, uint size, T* temp, bool CIsIdentity)
{
    enum
    {
        //! set exploration depth to a value ensuring that final accuracy is 0.5% in the worst case
        dichotomie = 10,
        //! Maximum number of iterations
        maxLoopCount = 10,
    };

    T rho = T(1);
    T top = T(1);
    T bot = T(0);
    T r;

    if (not Cholesky<T>(L, A, size, temp))
    {
        for (uint j = 0; j != size; ++j)
        {
            for (uint k = 0; k <= j; ++k)
            {
                B[j][k] = A[j][k];
            }
        }
        // A est definie positive ou semi-dp: pas de redressement à faire et LtL = A
        return T(1);
    }

    // Il faut trouver B definie positive et proche de A
    for (uint j = 0; j != size; ++j)
    {
        B[j][j] = T(1);
    }

    uint compteur = 0;
    do
    {
        for (uint i = 0; i != dichotomie; ++i)
        {
            r = (top + bot) / T(2);
            if (not CIsIdentity)
            {
                for (uint j = 0; j != size; ++j)
                {
                    for (uint k = 0; k < j; ++k)
                    {
                        B[j][k] = A[j][k] * r + C[j][k] * (T(1) - r);
                    }
                }
            }
            else
            {
                for (uint j = 0; j != size; ++j)
                {
                    for (uint k = 0; k < j; ++k)
                    {
                        B[j][k] = A[j][k] * r; // + C[j][k] * (T(1) - r);
                    }
                }
            }

            if (Cholesky<T>(L, B, size, temp))
            {
                top = r;
            }
            else
            {
                bot = r;
                rho = bot;
            }
        }

        if (rho < T(1))
        {
            // la derniere matrice n'etait pas bonne, il faut recalculer
            if (r != rho)
            {
                if (!CIsIdentity)
                {
                    for (uint j = 0; j != size; ++j)
                    {
                        for (uint k = 0; k < j; ++k)
                        {
                            B[j][k] = A[j][k] * rho + C[j][k] * (T(1) - rho);
                        }
                    }
                }
                else
                {
                    for (uint j = 0; j != size; ++j)
                    {
                        for (uint k = 0; k < j; ++k)
                        {
                            B[j][k] = A[j][k] * rho; // + C[j][k] * (T(1) - rho);
                        }
                    }
                }
                Cholesky<T>(L, B, size, temp);
            }
            return rho;
        }

        // avoid infinite loops
        if (++compteur > maxLoopCount) // si on n'a pas eu convergence C n'est pas valide
        {
            return T(-1);
        }
    } while (true);

    return rho;
}

} // namespace Solver
} // namespace Antares

#endif // __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_HXX__
