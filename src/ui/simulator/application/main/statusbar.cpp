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

#include <yuni/yuni.h>
#include "main.h"
#include "../../windows/version.h"
#include "../study.h"
#include "../../toolbox/components/datagrid/gridhelper.h"
#include <wx/statusbr.h>
#include <ui/common/lock.h>

using namespace Component::Datagrid;

namespace Antares::Forms
{

void ApplWnd::resetDefaultStatusBarText()
{
    assert(wxIsMainThread() == true && "Must be ran from the main thread");
#if defined(wxUSE_STATUSBAR)
    SetStatusText(wxString(wxT("  ")) << Antares::VersionToWxString());
#endif
}


bool oneCellSelected(wxGrid& grid)
{
    const wxGridCellCoordsArray& cells(grid.GetSelectedCells());
    return cells.size() > 0;
}

size_t updateStatisticsOpForOneCell(wxGrid& grid,
                                    VGridHelper* gridHelper,
                                    Selection::IOperator* op)
{
    size_t totalCell = 0;
    const wxGridCellCoordsArray& cells(grid.GetSelectedCells());
    for (uint i = 0; i < (uint)cells.size(); ++i)
    {
        auto& cell = cells[i];
        op->appendValue(gridHelper->GetNumericValue(cell.GetRow(), cell.GetCol()));
        ++totalCell;
    }
    return totalCell;
}

bool rowsSelected(wxGrid& grid)
{
    const wxArrayInt& rows(grid.GetSelectedRows());
    return rows.size() > 0;
}

size_t updateStatisticsOpForRows(wxGrid& grid,
                                 VGridHelper* gridHelper,
                                 Selection::IOperator* op)
{
    size_t totalCell = 0;
    int colCount = grid.GetNumberCols();
    const wxArrayInt& rows(grid.GetSelectedRows());
    for (uint i = 0; i < (uint)rows.size(); ++i)
    {
        for (int col = 0; col < colCount; ++col)
        {
            op->appendValue(gridHelper->GetNumericValue(rows[i], col));
            ++totalCell;
        }
    }
    return totalCell;
}

bool columnsSelected(wxGrid& grid)
{
    const wxArrayInt& cols(grid.GetSelectedCols());
    return cols.size() > 0;
}

size_t updateStatisticsOpForColumns(wxGrid& grid,
                                    VGridHelper* gridHelper,
                                    Selection::IOperator* op)
{
    size_t totalCell = 0;
    int rowCount = grid.GetNumberRows();
    const wxArrayInt& cols(grid.GetSelectedCols());
    for (uint i = 0; i < (uint)cols.size(); ++i)
    {
        for (int row = 0; row < rowCount; ++row)
        {
            op->appendValue(gridHelper->GetNumericValue(row, cols[i]));
            ++totalCell;
        }
    }
    return totalCell;
}

bool blockSelected(wxGrid& grid)
{
    // Blocks. We always expect blocks top left and bottom right to have the same size, since their
    // entries are supposed to correspond.
    const wxGridCellCoordsArray& blockTopLeft(grid.GetSelectionBlockTopLeft());
    const wxGridCellCoordsArray& blockBottomRight(grid.GetSelectionBlockBottomRight());
    return (blockTopLeft.size() == blockBottomRight.size()) && (blockTopLeft.size() > 0);
}

size_t updateStatisticsOpForBlock(wxGrid& grid,
                                  VGridHelper* gridHelper,
                                  Selection::IOperator* op)
{
    size_t totalCell = 0;
    const wxGridCellCoordsArray& blockTopLeft(grid.GetSelectionBlockTopLeft());
    const wxGridCellCoordsArray& blockBottomRight(grid.GetSelectionBlockBottomRight());
    size_t blockSize = blockTopLeft.size();

    for (uint i = 0; i < blockSize; ++i)
    {
        const wxGridCellCoords& topLeft = blockTopLeft[i];
        const wxGridCellCoords& bottomRight = blockBottomRight[i];
        for (int row = topLeft.GetRow(); row <= bottomRight.GetRow(); ++row)
        {
            for (int col = topLeft.GetCol(); col <= bottomRight.GetCol(); ++col)
            {
                op->appendValue(gridHelper->GetNumericValue(row, col));
                ++totalCell;
            }
        }
    }
    return totalCell;
}

/*
** Applies a functor to all selected cells. Returns the number of selected
** cells.
*/
static size_t applyOperatorOnSelectedCells(wxGrid& grid,
                                           VGridHelper* gridHelper,
                                           Selection::IOperator* op)
{
    assert(wxIsMainThread() == true and "Must be ran from the main thread");

    if (oneCellSelected(grid))
    {
        return updateStatisticsOpForOneCell(grid, gridHelper, op);
    }

    if (rowsSelected(grid))
    {
        return updateStatisticsOpForRows(grid, gridHelper, op);
    }
    
    if (columnsSelected(grid))
    {
        return updateStatisticsOpForColumns(grid, gridHelper, op);
    }

    if (blockSelected(grid))
    {
        return updateStatisticsOpForBlock(grid, gridHelper, op);
    }
    
    return 0;
}

void ApplWnd::gridOperatorSelectedCellsUpdateResult(wxGrid* grid)
{
    assert(wxIsMainThread() == true and "Must be ran from the main thread");
    enum
    {
        fieldIndex = 1,
    };
    // The status bar
    auto* statusBar = GetStatusBar();
    pGridSelectionAttachedGrid = grid;
    if (statusBar)
    {
        if (not CurrentStudyIsValid())
        {
            statusBar->SetStatusText(wxEmptyString, fieldIndex);
            return;
        }
        if (not pGridSelectionOperator)
        {
            statusBar->SetStatusText(wxT("|   (none)"), fieldIndex);
            return;
        }

        if (grid and grid->GetTable())
        {
            auto* gridHelper = dynamic_cast<Component::Datagrid::VGridHelper*>(grid->GetTable());
            if (gridHelper)
            {
                // Reset of the operator
                pGridSelectionOperator->reset();
                // Browse all selected cells
                if (applyOperatorOnSelectedCells(*grid, gridHelper, pGridSelectionOperator))
                {
                    // Update the GUI
                    statusBar->SetStatusText(wxString()
                                               << wxT("|  ") << pGridSelectionOperator->caption()
                                               << wxT(" = ") << pGridSelectionOperator->result(),
                                             fieldIndex);
                    return;
                }
            }
        }
        // Empty
        statusBar->SetStatusText(
          wxString(wxT("|   (")) << pGridSelectionOperator->caption() << wxT(')'), fieldIndex);
    }
}

void ApplWnd::evtOnContextMenuStatusBar(wxContextMenuEvent& evt)
{
    if (GUIIsLock())
        return;

    wxStatusBar* statusBar = GetStatusBar();
    if (statusBar)
    {
        wxRect rect;
        if (statusBar->GetFieldRect(1, rect))
        {
            const wxPoint pos = statusBar->ScreenToClient(evt.GetPosition());
            if (rect.Contains(pos))
            {
                if (!pPopupMenuOperatorsGrid)
                {
                    // Popup menu: Operators for selected cells on any grid
                    pPopupMenuOperatorsGrid = createPopupMenuOperatorsOnGrid();
                }

                statusBar->PopupMenu(pPopupMenuOperatorsGrid);
            }
        }
    }
    evt.Skip();
}

} // namespace Antares::Forms