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

#include <ui/common/wx-wrapper.h>
#include "wait.h"
#include <cassert>
#include "main/main.h"
#include <yuni/core/atomic/int.h>
#include <antares/logs/logs.h>

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
