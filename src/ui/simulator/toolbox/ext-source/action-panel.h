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
#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__

#include <action/action.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <ui/common/component/panel.h>

namespace Antares
{
namespace Private
{
namespace Window
{
class ActionPanel final : public Antares::Component::Panel
{
public:
    //! Array
    using Vector = std::vector<ActionPanel*>;

    enum
    {
        //! Optimal height for a single item
        itemHeight = 20,
    };

    static void DrawBackgroundWithoutItems(wxWindow& obj, wxDC& dc, const wxRect& rect);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ActionPanel(wxWindow* parent,
                ActionPanel* parentPanel,
                const Antares::Action::Context::Ptr& context,
                const Antares::Action::IAction::Ptr& action);
    //! Destructor
    virtual ~ActionPanel();
    //@}

    void expand();

    void collapse();

    void onDraw(wxPaintEvent&);

    void update();

    void forceUpdate();

    virtual void SetFocus();

private:
    void onEnter(wxMouseEvent&);
    void onLeave(wxMouseEvent&);
    void onMouseDown(wxMouseEvent&);
    void onMouseDownCollapseExpand();
    void onMouseDownBehaviorSelect(wxWindow* obj);
    void drawArrows(wxDC& dc, const wxRect& rect);

    void prepareAll(bool force = false);

    void relayoutAllParents();
    void computeTotalChildrenCount();

    void onBehaviorMerge(wxCommandEvent&);
    void onBehaviorOverwrite(wxCommandEvent&);
    void onBehaviorSkip(wxCommandEvent&);

public:
    ActionPanel* pParent;
    Antares::Action::Context::Ptr pContext;
    Antares::Action::IAction::Ptr pAction;
    bool pCollapsed;
    bool pBold;
    wxColour pBackgroundColor;
    wxColour pBackgroundColorLight;
    wxColour pStateColor[Antares::Action::stMax];
    wxColour pLineColor;
    uint pDepthSpace;
    wxString pText;
    wxString pBehaviorText;
    wxString pStateText;
    wxString pComments;
    bool pHasChildren;
    bool pCanDoSomething;
    bool phasCreatedChildren;
    static ActionPanel* SelectedItem;

    bool pDisabled;
    wxMenu* pPopupMenu;

    Antares::Action::State pState;
    Antares::Action::Behavior pBehavior;

    Vector pChildren;
    //! The total of all children for all sub-nodes
    uint pTotalChildrenCount;

    // Event table
    DECLARE_EVENT_TABLE()

}; // class ActionPanel

} // namespace Window
} // namespace Private
} // namespace Antares

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__
