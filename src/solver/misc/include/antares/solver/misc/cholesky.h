// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_MISC_CHOLESKY_H__
#define __ANTARES_SOLVER_MISC_CHOLESKY_H__

#include <yuni/yuni.h>
#include <yuni/core/math.h>

#include <antares/array/matrix.h>

namespace Antares::Solver
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

} // namespace Antares::Solver

#include "cholesky.hxx"

#endif // __ANTARES_SOLVER_MISC_CHOLESKY_H__
