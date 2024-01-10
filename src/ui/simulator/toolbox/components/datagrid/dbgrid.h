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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__

#include <wx/grid.h>
#include <yuni/yuni.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
// Forward declaration
class Component;

class DBGrid final : public wxGrid
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    DBGrid(Component* parent);
    //! Destructor
    virtual ~DBGrid();
    //@}

    const wxPoint& currentPosition() const
    {
        return pCurrentPosition;
    }

    Component* component() const
    {
        return pParentComponent;
    }

    void copyToClipboard();
    void copyAllToClipboard();
    void pasteFromClipboard();

    void ensureDataAreLoaded();

    void resizeAllHeaders(bool nodelay = false);

    /*!
    ** \brief Allow refresh
    */
    void enableRefresh(bool enabled)
    {
        pAllowRefresh = enabled;
    }

    bool canRefresh() const
    {
        return pAllowRefresh;
    }

    void DrawColLabel(wxDC& dc, int col, uint& offset);
    void DrawRowLabel(wxDC& dc, int row, uint& offset);
    void DrawCellHighlight(wxDC& dc, const wxGridCellAttr* attr);

    void disableColorMappingForRowLabels()
    {
        pColorMappingRowLabels = false;
    }

    // For synchronizing scroll with another grid purpose
    void setOtherGrid(DBGrid* otherGrid);

private:
    //! Get if the data are ready for the grid
    bool dataAreReady() const;

    void ensureDataAreLoadedDelayed();

    /*!
    ** \\brief Event: Some cells have been selected on the grid
    */
    void onGridSelectCell(wxGridEvent& evt);

    /*!
    ** \brief Event: Some cells have been selected on the grid
    */
    void onGridRangeSelect(wxGridRangeSelectEvent& evt);

    /*!
    ** \brief Event: The grid lost its focus
    */
    void onGridLeave(wxFocusEvent& evt);

    void onDraw(wxPaintEvent& evt);

    void onKeyUp(wxKeyEvent& evt);

    void evtOnResizeHeaders(wxCommandEvent& evt);

    void evtCornerPaint(wxPaintEvent& evt);

    void onEraseBackground(wxEraseEvent&)
    {
    }
    void onDrawColLabels(wxPaintEvent&);
    void onDrawRowLabels(wxPaintEvent&);

    void onScroll(wxScrollWinEvent&);

private:
    //! Parent component
    Component* pParentComponent;
    wxPoint pCurrentPosition;
    bool pAllowRefresh;
    bool pColorMappingRowLabels;
    // The other grid we want scroll synchronously with
    DBGrid* otherGrid_ = nullptr;

    DECLARE_EVENT_TABLE();

}; // class DBGrid

} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__
