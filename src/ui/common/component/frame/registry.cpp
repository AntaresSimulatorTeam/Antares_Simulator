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

#include <yuni/yuni.h>
#include "registry.h"
#include <vector>
#include "frame.h"
#include "../../dispatcher.h"
#include "local-frame.h"

namespace Antares
{
namespace Component
{
namespace Frame
{
namespace Registry
{
static IFrame::Vector gsFrames;

static void DelayedDispatch()
{
    auto end = gsFrames.end();
    for (auto it = gsFrames.begin(); it != end; ++it)
        (*it)->updateOpenWindowsMenu();
}

static void DelayedDispatchWithExclude(IFrame* exclude)
{
    auto end = gsFrames.end();
    for (auto it = gsFrames.begin(); it != end; ++it)
    {
        auto* frame = *it;
        if (frame != exclude)
            frame->updateOpenWindowsMenu();
    }
}

void RegisterFrame(IFrame* frame)
{
    auto found = std::find(gsFrames.begin(), gsFrames.end(), frame);
    if (found == gsFrames.end())
        gsFrames.push_back(frame);
    DelayedDispatchWithExclude(frame);
}

void UnregisterFrame(IFrame* frame)
{
    auto found = find(gsFrames.begin(), gsFrames.end(), frame);
    if (found != gsFrames.end())
    {
        gsFrames.erase(found);
        DispatchUpdate();
        DelayedDispatch();
    }
}

void CloseAllLocal()
{
    IFrame::Vector copy(gsFrames);

    // Closing all local frames in the reverse order of their creation
    for (auto it = copy.rbegin(); it != copy.rend(); ++it)
    {
        auto* frame = dynamic_cast<WxLocalFrame*>(*it);
        if (frame && not frame->excludeFromMenu())
            frame->Close();
    }
    Dispatcher::GUI::Post(&DelayedDispatch);
}

void DispatchUpdate()
{
    Dispatcher::GUI::Post(&DelayedDispatch);
}

void DispatchUpdate(IFrame* exclude)
{
    Yuni::Bind<void()> callback;
    callback.bind(DelayedDispatchWithExclude, exclude);
    Dispatcher::GUI::Post(callback);
}

const IFrame::Vector& List()
{
    return gsFrames;
}

} // namespace Registry
} // namespace Frame
} // namespace Component
} // namespace Antares
