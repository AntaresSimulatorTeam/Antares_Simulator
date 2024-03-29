/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "math.h"

namespace Yuni
{
namespace Math
{
/*!
** \brief The exponential function
*/
template<class T>
YUNI_DECL T Exp(T x);

} // namespace Math
} // namespace Yuni

#include "exponential.hxx"
