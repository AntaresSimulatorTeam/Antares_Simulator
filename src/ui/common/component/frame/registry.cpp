/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
