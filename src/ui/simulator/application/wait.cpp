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

#include <ui/common/wx-wrapper.h>
#include "wait.h"
#include <cassert>
#include "main/main.h"
#include <yuni/core/atomic/int.h>
#include <antares/logs.h>

namespace Antares
{
namespace WIP
{
void Enter(bool immediate)
{
    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
    if (immediate)
        mainFrm.showWIP();
    else
        Dispatcher::GUI::Post(&mainFrm, &Forms::ApplWnd::showWIP);
}

void Leave()
{
    Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
    Dispatcher::GUI::Post(&mainFrm, &Forms::ApplWnd::hideWIP);
}

} // namespace WIP
} // namespace Antares
