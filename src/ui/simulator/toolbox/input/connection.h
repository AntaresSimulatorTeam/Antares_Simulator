// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__
#define __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__

#include <wx/panel.h>
#include <antares/study/study.h>
#include "input.h"
#include <wx/arrstr.h>
#include <yuni/core/event.h>
#include <wx/treectrl.h>
#include <wx/combobox.h>

namespace Antares::Toolbox::InputSelector
{
class Connections: public AInput, public Yuni::IEventObserver<Connections>
{
public:
    Connections(wxWindow* parent);
    virtual ~Connections();

    virtual void update();

    virtual wxPoint recommendedSize() const
    {
        return wxPoint(250, 70);
    }

public:
    Yuni::Event<void(Antares::Data::AreaLink*)> onConnectionChanged;

protected:
    virtual void internalBuildSubControls();

private:
    void onStudyClosed();
    void onStudyAreaUpdate(Data::Area*);
    void onStudyLinkUpdate(Data::AreaLink*);

    void onSelectionChanged(wxTreeEvent& evt);
    void delayedSelection(wxTreeItemData* data);

    void onMapLayerAdded(const wxString* text);
    void onMapLayerRemoved(const wxString* text);
    void onMapLayerChanged(const wxString* text);
    void onMapLayerRenamed(const wxString* text);
    void layerFilterChanged(wxCommandEvent& evt);

private:
    wxComboBox* pLayerFilter;
    wxTreeCtrl* pListbox;
    Antares::Data::AreaLink* pLastSelected;

}; // class Area

} // namespace Antares::Toolbox::InputSelector

#endif // __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__
