// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__

#include <action/action.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <ui/common/component/panel.h>

namespace Antares::Private::Window
{
class ActionPanel final: public Antares::Component::Panel
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

} // namespace Antares::Private::Window

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_ACTION_PANEL_H__
