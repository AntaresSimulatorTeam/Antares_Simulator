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
