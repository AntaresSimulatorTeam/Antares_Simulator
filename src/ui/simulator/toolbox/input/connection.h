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
#ifndef __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__
#define __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__

#include <antares/wx-wrapper.h>
#include <wx/panel.h>
#include <antares/study.h>
#include "input.h"
#include <wx/arrstr.h>
#include <yuni/core/event.h>
#include <wx/treectrl.h>
#include <wx/combobox.h>

namespace Antares
{
namespace Toolbox
{
namespace InputSelector
{
class Connections : public AInput, public Yuni::IEventObserver<Connections>
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

} // namespace InputSelector
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_INPUT_CONNECTIONS_H__
