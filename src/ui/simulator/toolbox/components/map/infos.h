/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_MAP_PANEL_INFOS_H__
#define __ANTARES_TOOLBOX_MAP_PANEL_INFOS_H__

#include <wx/panel.h>
#include <wx/sizer.h>
#include "nodes/item.h"
#include <wx/clrpicker.h>
#include <wx/checkbox.h>

namespace Antares
{
namespace Map
{
/*!
** \brief Informations about the selected items on the map
*/
class PanelInfos : public wxPanel
{
public:
    //! List of item
    using ItemList = std::vector<Item*>;
    //! IDs
    enum IDs
    {
        idChangeColor = 10000,
        idDefaultColor,
        idOrientation,
        idAreaName,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    ** \param parent The parent window
    */
    PanelInfos(wxWindow* parent);
    //! Destructor
    virtual ~PanelInfos();
    //@}

    /*!
    ** \brief Refresh all information about a list of nodes
    **
    ** The refresh will ionly occur if the list is not the same than the previous
    ** given one. This list should be a list of currently selected nodes.
    **
    ** \param newList The new list of selected items
    ** \param force True to force the refresh
    */
    void refresh(ItemList& newList, const bool force = false);

private:
    /*!
    ** \brief Test if the given list is identical to the last one
    */
    bool listIsEquals(const ItemList& list) const;

    /*!
    ** \brief Create a component wxColourPickerCtrl, with a button to restore a default color
    */
    wxBoxSizer* createColorPicker(const wxColour& color);

    /*!
    ** \brief Get the number of node and connections from a list
    */
    void countItemSpecies(const ItemList& list, int& nodes, int& connections) const;

    /*!
    ** \brief Extract the common color from a list of nodes, black if could not be found
    */
    wxColour findCommonColour(const ItemList& list) const;

    /*!
    ** \brief Find the orientation of a list of connection
    */
    wxCheckBoxState findConnectionOrientation(const ItemList& list) const;

    void onChangeColor(wxColourPickerEvent& evt);
    void onChangeToDefaultColor(wxCommandEvent& evt);
    void onChangeOrientation(wxCommandEvent& evt);
    void onChangeAreaName(wxCommandEvent& evt);

    void layoutAddTitle(const wxString& title);

    //! Expanding the layout: No selection has been made
    void layoutNoSelection();
    //! Expanding the layout: Properties for the selected nodes
    void layoutPropertiesForNodes(const int nodeCount);
    //! Expanding the layout: Properties for the selected connections
    void layoutPropertiesForConnections(const int connectionCount);
    //! Expanding the layout: Filters for the selected nodes
    void layoutFiltersForNodes(const int nodeCount);
    //! Expanding the layout: Filters for the selected connections
    void layoutFiltersForConnections(const int connectionCount);

private:
    /*!
    ** \brief List of pointers of selected items
    ** \internal This list is used to avoid useless updates. It is also used
    **    to determine what kind of items we have
    */
    ItemList pItemList;
    //! Color picker, to change the color of area(s)
    wxColourPickerCtrl* pColorPicker;

    // Event table
    DECLARE_EVENT_TABLE()

}; // class PanelInfos

} // namespace Map
} // namespace Antares

#endif // __ANTARES_TOOLBOX_MAP_PANEL_INFOS_H__
