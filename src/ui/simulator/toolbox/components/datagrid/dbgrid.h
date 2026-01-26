// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__

#include <wx/grid.h>
#include <yuni/yuni.h>

namespace Antares::Component::Datagrid
{
// Forward declaration
class Component;

class DBGrid final: public wxGrid
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

} // namespace Antares::Component::Datagrid

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_DBGRID_H__
