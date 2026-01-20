// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "output.h"

using namespace Yuni;

namespace Antares::Window::OutputViewer
{
Component::Tab::Tab(Component& comparent):
    component(comparent),
    support(nullptr),
    btnOutput(nullptr),
    btnItem(nullptr)
{
}

Component::Tab::~Tab()
{
    assert(support && "invalid support ?");
    if (support)
    {
        if (component.pSizerForBtnOutputs)
        {
            component.pSizerForBtnOutputs->Detach(support);
        }
        support->Destroy();
    }
}

void Component::Tab::updateAreaOrLinkName(SelectionType selection, const String& text)
{
    if (btnItem)
    {
        switch (selection)
        {
        case stNone:
            btnItem->image("images/16x16/empty.png");
            btnItem->caption(wxT("(none)"));
            break;
        case stArea:
            if (text.contains("@ "))
            {
                btnItem->image("images/16x16/group.png");
            }
            else
            {
                btnItem->image("images/16x16/area.png");
            }
            btnItem->caption(wxStringFromUTF8(text));
            break;
        case stLink:
            btnItem->image("images/16x16/link.png");
            btnItem->caption(wxStringFromUTF8(text));
            break;
        case stLogs:
            btnItem->image("images/16x16/logs.png");
            btnItem->caption(wxT("Logs"));
            break;
        case stSummary:
            btnItem->image("images/16x16/property.png");
            btnItem->caption(wxT("Summary"));
            break;
        case stComments:
            btnItem->image("images/16x16/notes.png");
            btnItem->caption(wxT("Comments"));
            break;
        }

        assert(btnItem->GetParent());
        auto* sizer = btnItem->GetParent();
        if (sizer)
        {
            sizer->Layout();
        }
    }

    assert(support);
    assert(support->GetSizer());
    if (support)
    {
        auto* sizer = support->GetSizer();
        if (sizer)
        {
            sizer->Layout();
        }
    }
}

} // namespace Antares::Window::OutputViewer
