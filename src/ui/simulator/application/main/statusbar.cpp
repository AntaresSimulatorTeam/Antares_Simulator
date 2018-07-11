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

#include <yuni/yuni.h>
#include "main.h"
#include "../../windows/version.h"
#include "../study.h"
// Datagrid
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/datagrid/gridhelper.h"
#include <wx/statusbr.h>
#include <ui/common/lock.h>




namespace Antares
{
namespace Forms
{


	void ApplWnd::resetDefaultStatusBarText()
	{
		assert(wxIsMainThread() == true && "Must be ran from the main thread");
		# if defined(wxUSE_STATUSBAR)
		SetStatusText(wxString(wxT("  ")) << Antares::VersionToWxString());
		# endif
	}


	/*
	** Applies a functor to all selected cells. Returns the number of selected
	** cells.
	*/
	static size_t ForAllSelectedCells(wxGrid& grid, Component::Datagrid::VGridHelper* gridHelper,
		Component::Datagrid::Selection::IOperator* op)
	{
		assert(wxIsMainThread() == true and "Must be ran from the main thread");

		size_t totalCell = 0;

		// Singly selected cells.
		const wxGridCellCoordsArray& cells(grid.GetSelectedCells());
		for (uint i = 0; i < (uint) cells.size(); ++i)
		{
			auto& cell = cells[i];
			op->appendValue(gridHelper->GetNumericValue(cell.GetRow(), cell.GetCol()));
			++totalCell;
		}

		// Whole selected rows.
		int colCount = grid.GetNumberCols();
		const wxArrayInt& rows(grid.GetSelectedRows());
		for (uint i = 0; i < (uint) rows.size(); ++i)
		{
			for (int col = 0; col < colCount; ++col)
			{
				op->appendValue(gridHelper->GetNumericValue(rows[i], col));
				++totalCell;
			}
		}

		// Whole selected columns.
		int rowCount = grid.GetNumberRows();
		const wxArrayInt& cols(grid.GetSelectedCols());
		for (uint i = 0; i < (uint) cols.size(); ++i)
		{
			for (int row = 0; row < rowCount; ++row)
			{
				op->appendValue(gridHelper->GetNumericValue(row, cols[i]));
				++totalCell;
			}
		}

		// Blocks. We always expect btl and bbr to have the same size, since their
		// entries are supposed to correspond.
		const wxGridCellCoordsArray& btl(grid.GetSelectionBlockTopLeft());
		const wxGridCellCoordsArray& bbr(grid.GetSelectionBlockBottomRight());
		size_t count = btl.size();
		if (count == bbr.size())
		{
			for (uint i = 0; i < count; ++i)
			{
				const wxGridCellCoords& tl = btl[i];
				const wxGridCellCoords& br = bbr[i];
				for (int row = tl.GetRow(); row <= br.GetRow(); ++row)
				{
					for (int col = tl.GetCol(); col <= br.GetCol(); ++col)
					{
						op->appendValue(gridHelper->GetNumericValue(row, col));
						++totalCell;
					}
				}
			}
		}
		return totalCell;
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
			if (not Data::Study::Current::Valid())
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
					if (ForAllSelectedCells(*grid, gridHelper, pGridSelectionOperator))
					{
						// Update the GUI
						statusBar->SetStatusText(wxString() << wxT("|  ")
							<< pGridSelectionOperator->caption() << wxT(" = ")
							<< pGridSelectionOperator->result(), fieldIndex);
						return;
					}
				}
			}
			// Empty
			statusBar->SetStatusText(wxString(wxT("|   ("))
				<< pGridSelectionOperator->caption() << wxT(')'), fieldIndex);
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





} // namespace Forms
} // namespace Antares

