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

#include "output.h"

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
Component::Tab::Tab(Component& comparent) :
 component(comparent), support(nullptr), btnOutput(nullptr), btnItem(nullptr)
{
}

Component::Tab::~Tab()
{
    assert(support && "invalid support ?");
    if (support)
    {
        if (component.pSizerForBtnOutputs)
            component.pSizerForBtnOutputs->Detach(support);
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
                btnItem->image("images/16x16/group.png");
            else
                btnItem->image("images/16x16/area.png");
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
            sizer->Layout();
    }

    assert(support);
    assert(support->GetSizer());
    if (support)
    {
        auto* sizer = support->GetSizer();
        if (sizer)
            sizer->Layout();
    }
}

} // namespace OutputViewer
} // namespace Window
} // namespace Antares
