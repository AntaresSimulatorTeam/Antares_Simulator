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
#ifndef __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
#define __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__

#include <antares/wx-wrapper.h>
#include "../toolbox/components/datagrid/component.h"
#include "../toolbox/input/connection.h"
#include "../toolbox/components/datagrid/renderer/connection.h"
#include "../toolbox/components/button.h"
#include <wx/sizer.h>

namespace Antares
{
namespace Window
{
/*!
**
*/
class Interconnection : public wxScrolledWindow, public Yuni::IEventObserver<Interconnection>
{
public:
    //! \name Constructor & Destructor
    //@{
    Interconnection(wxWindow* parent, Toolbox::InputSelector::Connections* notifier);
    //! Destructor
    virtual ~Interconnection();
    //@}

private:
    /*!
    ** \brief Event: The selection of the connection has changed
    */
    void onConnectionChanged(Data::AreaLink* link);

    void onPopupMenuTransmissionCapacities(Component::Button&, wxMenu& menu, void*);
    void onSelectTransCapInclude(wxCommandEvent&);
    void onSelectTransCapIgnore(wxCommandEvent&);
    void onSelectTransCapInfinite(wxCommandEvent&);

    void onPopupMenuAssetType(Component::Button&, wxMenu& menu, void*);
    void onSelectAssetTypeAC(wxCommandEvent&);
    void onSelectAssetTypeDC(wxCommandEvent&);
    void onSelectAssetTypeGas(wxCommandEvent&);
    void onSelectAssetTypeVirt(wxCommandEvent&);
    void onSelectAssetTypeOther(wxCommandEvent&);

    void onPopupMenuHurdlesCosts(Component::Button&, wxMenu& menu, void*);
    void onSelectIncludeHurdlesCosts(wxCommandEvent&);
    void onSelectIgnoreHurdlesCosts(wxCommandEvent&);

    void onPopupMenuLink(Component::Button&, wxMenu& menu, void*);
    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

    void onStudyLinkChanged(Data::AreaLink* link);

private:
    //! Pointer to the current link
    Data::AreaLink* pLink;
    //! Button which display the name of the current link
    Component::Button* pLinkName;
    //! Hudrles costs
    Component::Button* pHurdlesCost;
    //! Loop flow
    Component::Button* pLoopFlow;
    // !Phase shifter
    Component::Button* pPhaseShift;
    //! Copper Plate
    Component::Button* pCopperPlate;
    //! Asset type
    Component::Button* pAssetType;
    //! Caption
    wxWindow* pCaptionText;
    //! No Link
    wxWindow* pNoLink;
    //! Link data
    wxWindow* pLinkData;

    //! The main grid sizer
    wxSizer* pGridSizer;
    //!
    wxWindow* pLabelCaption;
    wxSizer* pCaptionDataSizer;

}; // class Interconnection

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_INTERCONNECTIONS_H__
