/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

//! Precision sur la factorisation des matrices
#define ANTARES_CHOLESKY_EPSIMIN ((T)1.0e-9)

namespace Antares
{
namespace Solver
{
template<class T, class U1, class U2>
bool Cholesky(U1& L, U2& A, uint size, T* temp)
{
    using namespace Yuni;

    for (uint i = 0; i != size; ++i)
        temp[i] = 0;

    T som;

    for (uint i = 0; i < size; ++i)
    {
        typename MatrixSubColumn<U1>::Type Li = L[i];

        // on calcule d'abord L[i][i]
        som = A[i][i];
        for (int j = 0; j <= (int)(i - 1); ++j)
            som -= Li[j] * Li[j];

        if (som > ANTARES_CHOLESKY_EPSIMIN)
        {
            Li[i] = Math::SquareRootNoCheck(som);

            // maintenant on cherche L[k][i], k > i.
            for (uint k = i + 1; k < size; ++k)
            {
                typename MatrixSubColumn<U1>::Type Lk = L[k];
                typename MatrixSubColumn<U2>::Type Ak = A[k];

                if (temp[k] == Ak[k])
                {
                    Lk[i] = 0;
                }
                else
                {
                    som = Ak[i];
                    for (int j = 0; j <= (int)(i - 1); ++j)
                        som -= Li[j] * Lk[j];

                    Lk[i] = som / Li[i];
                    temp[k] += Lk[i] * Lk[i];

                    // si temp[k] = A[k][k] la matrice n'est pas dp mais il est encore possible
                    // qu'elle soit sdp.
                    // si temp > A[k][k] alors il est certain que A n'est ni sdp ni dp donc on
                    // arrete le calcul
                    if (temp[k] > Ak[k])
                        return true;
                }
            }
        }
        else
        {
            // annule le reste de la colonne
            for (uint k = i; k != size; ++k)
                L[k][i] = 0;
        }
    }

    return false;
}

} // namespace Solver
} // namespace Antares

#undef ANTARES_CHOLESKY_EPSIMIN
