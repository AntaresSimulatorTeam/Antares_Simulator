/*
** Copyright 2007-2023 RTE
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

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include "xcast.h"
#include <yuni/core/math.h>
#include <antares/logs/logs.h>

using namespace Yuni;

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{
double XCast::GammaInc(double s, double z)
{
    enum
    {
        length = 30,
    };

    double x = 1. / z;
    double somme = 0.;
    for (int i = 0; i < length + 1; ++i)
    {
        x *= z;
        x /= (s + double(i));
        somme += x;
    }
    somme *= exp(-z);
    somme *= pow(z, s);
    return somme;
}

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares
