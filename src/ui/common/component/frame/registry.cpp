// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <yuni/yuni.h>
#include "registry.h"
#include <vector>
#include "frame.h"
#include "../../dispatcher.h"
#include "local-frame.h"

namespace Antares::Component::Frame::Registry
{
static IFrame::Vector gsFrames;

static void DelayedDispatch()
{
    auto end = gsFrames.end();
    for (auto it = gsFrames.begin(); it != end; ++it)
    {
        (*it)->updateOpenWindowsMenu();
    }
}

static void DelayedDispatchWithExclude(IFrame* exclude)
{
    auto end = gsFrames.end();
    for (auto it = gsFrames.begin(); it != end; ++it)
    {
        auto* frame = *it;
        if (frame != exclude)
        {
            frame->updateOpenWindowsMenu();
        }
    }
}

void RegisterFrame(IFrame* frame)
{
    auto found = std::find(gsFrames.begin(), gsFrames.end(), frame);
    if (found == gsFrames.end())
    {
        gsFrames.push_back(frame);
    }
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
        {
            frame->Close();
        }
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

} // namespace Antares::Component::Frame::Registry
