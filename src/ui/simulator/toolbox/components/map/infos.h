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
#ifndef __ANTARES_TOOLBOX_MAP_PANEL_INFOS_H__
#define __ANTARES_TOOLBOX_MAP_PANEL_INFOS_H__

#include <antares/wx-wrapper.h>
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
    typedef std::vector<Item*> ItemList;
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
