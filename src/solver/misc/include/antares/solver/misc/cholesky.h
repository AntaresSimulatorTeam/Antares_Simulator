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
#ifndef __ANTARES_SOLVER_MISC_CHOLESKY_H__
#define __ANTARES_SOLVER_MISC_CHOLESKY_H__

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/array/matrix.h>

namespace Antares
{
namespace Solver
{
/*!
** \brief Factorisation de Cholesky d'une matrice A symetrique reelle
**
** Factorisation de Cholesky d'une matrice A symetrique reelle dont ignore si
** elle est ou non semi definie positive.
** Si A est definie positive ou semi-definie positive on acheve la factorisation
** Si A n'est pas definie positive on arrete la factorisation en cours de route
** On retourne un code qui indique si A est définie positive ou non
**
**   LtL = A si A est semi-definie positive
** \param      A matrice NxN a factoriser
**
** \param L A bi-dimensional array of the form T**
** \param A A bi-dimensional array of the form T**
** \param size The number of process
** \param temp A temporary array for calculations (with at least @size items)
** \return true si A n'est ni definie positive ni semi-positive, false sinon
*/
template<class T, class U1, class U2>
bool Cholesky(U1& L, U2& A, uint size, T* temp);

} // namespace Solver
} // namespace Antares

#include "cholesky.hxx"

#endif // __ANTARES_SOLVER_MISC_CHOLESKY_H__
