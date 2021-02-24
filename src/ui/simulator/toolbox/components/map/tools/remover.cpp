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

#include "remover.h"
#include "../settings.h"
#include "../../../../windows/message.h"
#include "../../../../application/main/main.h"
#include "../../../../application/study.h"
#include "../../../../windows/inspector.h"

using namespace Yuni;

namespace Antares
{
namespace Map
{
namespace Tool
{
Remover::Remover(Manager& manager) : Tool(manager, nullptr)
{
    pWidth = 20;
    pHeight = 20;
}

Remover::~Remover()
{
}

void Remover::draw(DrawingContext& dc,
                   const bool mouseDown,
                   const wxPoint& position,
                   const wxPoint& absolute) const
{
    // Parent::draw
    Tool::draw(dc, mouseDown, position, absolute);
}

bool Remover::onMouseUp(const int, const int)
{
    if (not Data::Study::Current::Valid())
        return false;

    auto& mainFrm = *Forms::ApplWnd::Instance();

    wxString messageText
      = wxT("Do you really want to delete the selected items ?\nSelected items: ");
    uint selectedAreasCount = Antares::Window::Inspector::SelectionAreaCount();
    if (selectedAreasCount > 0)
    {
        messageText << selectedAreasCount;
        messageText << " Area";
        if (selectedAreasCount > 1)
            messageText << "s";
    }

    uint selectedLinksCount = Antares::Window::Inspector::SelectionLinksCount();
    if (selectedLinksCount > 0)
    {
        if (selectedAreasCount > 0)
            messageText << ", ";
        messageText << selectedLinksCount;
        messageText << " Link";
        if (selectedLinksCount > 1)
            messageText << "s";
    }

    uint selectedConstraintsCount = Antares::Window::Inspector::SelectionBindingConstraintCount();
    if (selectedConstraintsCount > 0)
    {
        if (selectedLinksCount > 0 || selectedAreasCount > 0)
            messageText << ", ";
        messageText << selectedConstraintsCount;
        messageText << " Constraint";
        if (selectedConstraintsCount > 1)
            messageText << "s";
    }

    Window::Message message(&mainFrm, wxT("Map"), wxT("Map"), messageText);
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        // Remove all selected items
        bool r = (0 != pManager.removeAllSelected());

        // post-check about the study - paranoid
        if (not Data::Study::Current::Valid())
            return false;

        // Force the refresh of runtime data
        logs.debug() << "  Asking to reload UI runtime data";
        auto* info = Data::Study::Current::Get()->uiinfo;
        if (info)
        {
            info->reload();
            info->reloadBindingConstraints();
        }
        return r;
    }
    return false;
}

} // namespace Tool
} // namespace Map
} // namespace Antares
