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
#ifndef __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_H__
#define __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_H__

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include "cholesky.h"

namespace Antares
{
namespace Solver
{
/*!
** \brief Transformation d'une matrice de pseudo-correlation A
**
** Transformation d'une matrice de pseudo-correlation A en une matrice de
** correlation B obtenue par interpolation lineaire avec une matrice de
** correlation C : B = rho * A + (1 - rho) * C
**
** \warning Aucune verification n'est effectuee sur le fait que A et C sont
**   des matrices de pseudo-correlation.
** \warning Ce code ne verifie pas que C est bien une matrice de correlation dp
** \warning Les parametres L, A, B et C doivent etre de type T** (exemple: B[x][y])
**
** \param N
** \param A matrice N x N de pseudo-correlation (Aii=1 et -1<=Aij<=1 pour i#j)
**   seuls les termes triangulaires inferieurs (diagonale incluse) sont à renseigner
** \param C matrice de correlation N X N definie positive (les matrices semmi-definies
**   positives ne sont pas admises) seuls les termes triangulaires inférieurs
**   (diagonale exclue) sont a renseigner
** \param[out] B matrice de correlation proche de A
**   seuls les termes triangulaires inferieurs (diagonale incluse)sont retournes
** \param[out] L matrice triangulaire inferieure telle que LtL = B
** \param CIsIdentity True to Consider C as an identity matrix
** \return valeur maximale de 0 <= r <= 1 t.q r * A +(1-r) * C definie positive
**    1 si A est une matrice de correlation
**   -1 si pas convergence (C n'est pas une matrice valide)
*/
template<class T, class U1, class U2, class U3, class U4>
T MatrixDPMake(U1& L, U2& A, U3& B, U4& C, unsigned int size, T* temp, bool CIsIdentity = false);

} // namespace Solver
} // namespace Antares

#include "matrix-dp-make.hxx"

#endif // __ANTARES_SOLVER_MISC_MATRIX_DP_MAKE_H__
