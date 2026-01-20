// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <antares/study/scenario-builder/updater.hxx>

#include "remover.h"
#include "../settings.h"
#include "../../../../windows/message.h"
#include "../../../../application/main/main.h"
#include "../../../../application/study.h"
#include "../../../../windows/inspector.h"
#include "antares/study/ui-runtimeinfos.h"

using namespace Yuni;

namespace Antares::Map::Tool
{
Remover::Remover(Manager& manager):
    Tool(manager, nullptr)
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
    if (not CurrentStudyIsValid())
    {
        return false;
    }

    auto& mainFrm = *Forms::ApplWnd::Instance();

    wxString messageText = wxT(
      "Do you really want to delete the selected items ?\nSelected items: ");
    uint selectedAreasCount = Antares::Window::Inspector::SelectionAreaCount();
    if (selectedAreasCount > 0)
    {
        messageText << selectedAreasCount;
        messageText << " Area";
        if (selectedAreasCount > 1)
        {
            messageText << "s";
        }
    }

    uint selectedLinksCount = Antares::Window::Inspector::SelectionLinksCount();
    if (selectedLinksCount > 0)
    {
        if (selectedAreasCount > 0)
        {
            messageText << ", ";
        }
        messageText << selectedLinksCount;
        messageText << " Link";
        if (selectedLinksCount > 1)
        {
            messageText << "s";
        }
    }

    uint selectedConstraintsCount = Antares::Window::Inspector::SelectionBindingConstraintCount();
    if (selectedConstraintsCount > 0)
    {
        if (selectedLinksCount > 0 || selectedAreasCount > 0)
        {
            messageText << ", ";
        }
        messageText << selectedConstraintsCount;
        messageText << " Constraint";
        if (selectedConstraintsCount > 1)
        {
            messageText << "s";
        }
    }

    Window::Message message(&mainFrm, wxT("Map"), wxT("Map"), messageText);
    message.add(Window::Message::btnYes);
    message.add(Window::Message::btnCancel, true);
    if (message.showModal() == Window::Message::btnYes)
    {
        ScenarioBuilderUpdater updaterSB(*GetCurrentStudy());
        // Remove all selected items
        bool r = (0 != pManager.removeAllSelected());

        // post-check about the study - paranoid
        if (not CurrentStudyIsValid())
        {
            return false;
        }

        // Force the refresh of runtime data
        logs.debug() << "  Asking to reload UI runtime data";
        auto* info = GetCurrentStudy()->uiinfo;
        if (info)
        {
            info->reload();
            info->reloadBindingConstraints();
        }
        return r;
    }
    return false;
}

} // namespace Antares::Map::Tool
