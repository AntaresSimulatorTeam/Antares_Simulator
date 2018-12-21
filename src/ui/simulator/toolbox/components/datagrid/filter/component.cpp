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

#include <antares/wx-wrapper.h>
#include "component.h"
#include "../gridhelper.h"
#include <wx/grid.h>
#include <antares/logs.h>


using namespace Yuni;


namespace Antares
{
namespace Toolbox
{
namespace Filter
{


	Component::Component(wxWindow* parent, Date::Precision precision)
		:Antares::Component::Panel(parent),
		pGrid(nullptr),
		pGridHelper(nullptr),
		pRefreshBatchCount(0),
		pPrecision(precision)
	{
		// Sizer
		SetSizer(new wxBoxSizer(wxVERTICAL));

		// A default input (-> Any)
		this->add();
	}


	Component::~Component()
	{
		// Clear all subfilters
		clear();
	}


	Input* Component::add()
	{
		// Creating the input
		Input* input = new Input(this);
		input->precision(pPrecision);
		input->addStdPreset();

		// Referencing the input
		pInputs.push_back(input);

		// Adding the input in the GUI
		if (GetSizer())
		{
			GetSizer()->Add(input, 0, wxALL|wxEXPAND, 1);
			GetSizer()->Layout();
		}

		if (!pInputs.empty())
		{
			Input* p = pInputs.front();
			if (p)
				p->showBtnToRemoveFilter(pInputs.size() > 1);
		}

		// Force the resize of the parent layout
		if (GetParent() && GetParent()->GetSizer())
			GetParent()->GetSizer()->Layout();
		return input;
	}


	bool Component::remove(Input* in)
	{
		// nullptr object, nothing to do and not considered as an error
		if (!in)
			return true;

		// The last input must not be deleted
		if (pInputs.size() == 1)
		{
			// Hiding the button
			pInputs.front()->showBtnToRemoveFilter(false);
			return false;
		}

		bool result = false;

		const InputList::iterator end = pInputs.end();
		for (InputList::iterator i = pInputs.begin(); i != end; ++i)
		{
			if (*i == in)
			{
				Input* input = *i;
				// Remove the item from the list
				pInputs.erase(i);
				// Detroy the input
				GetSizer()->Detach(input);
				input->Destroy();
				result = true;
				break;
			}
		}

		// Hiding the last button to remove the input filter
		if (!pInputs.empty())
			pInputs.front()->showBtnToRemoveFilter(pInputs.size() > 1);

		// The whole grid must be refreshed
		// Because the columns and/or the rows might have changed
		refresh();

		return result;
	}


	void Component::remove(int id)
	{
		const InputList::iterator end = pInputs.end();
		for (InputList::iterator i = pInputs.begin(); i != end; ++i)
		{
			Input* input = *i;
			if (input->id() == id)
			{
				remove(input);
				return;
			}
		}
	}


	void Component::clear()
	{
		// Clearing the list
		pInputs.clear();
		// Detaching all children from the main sizer
		wxSizer* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}


	void Component::classifyFilters(InputVector& onRows, InputVector& onCols, InputVector& onCells)
	{
		const InputList::iterator end = pInputs.end();
		for (InputList::iterator i = pInputs.begin(); i != end; ++i)
		{
			AFilterBase* f = (*i)->selected();
			if (f)
			{
				if (f->checkOnRowsLabels())
					onRows.push_back(*i);
				if (f->checkOnColsLabels())
					onCols.push_back(*i);
				if (f->checkOnCells())
					onCells.push_back(*i);
			}
		}
	}


	void Component::refresh()
	{
		// We count the number of batch, to avoid unnecessary refresh
		++pRefreshBatchCount;
		Dispatcher::GUI::Post(this, &Component::evtRefreshGrid);
	}


	void Component::evtRefreshGrid()
	{
		--pRefreshBatchCount;
		if (!pRefreshBatchCount && pGrid)
		{
			updateSearchResults();

			// Force the resize of the parent layout
			if (GetParent() && GetParent()->GetSizer())
				GetParent()->GetSizer()->Layout();

			pGrid->ForceRefresh();
			Refresh();
		}
	}



	void Component::updateSearchResults()
	{
		if (!pGrid || !pGridHelper)
			return;

		pGrid->BeginBatch();
		InputVector onRows;
		InputVector onCols;
		InputVector onCells;

		// For efficiency, it would be better if the filters eliminate rows and cols
		// in this order :
		// Rows, then cols, then cells
		classifyFilters(onRows, onCols, onCells);

		// In any cases, all indices must be reset to default
		// The algorithm may not manage all cases and the default values
		// are expected
		pGridHelper->resetIndicesToDefault();
		onUpdateSearchResults();
		// Something to do ?
		if (!onRows.empty() || !onCols.empty() || !onCells.empty())
		{
			// Prepare only the arrays
			/*pGridHelper->resetIndicesToDefaultWithoutInit();
			onUpdateSearchResults();*/
			if (!onRows.empty())
			{
				int last = 0; /* The count of valid rows */
				const InputVector::iterator end = onRows.end();
				bool good;
				for (int r = 0; r != pGridHelper->virtualSize.y; ++r)
				{
					good = true;
					for (InputVector::iterator i = onRows.begin(); i != end; ++i)
					{
						(*i)->selected()->dataGridPrecision(pDataGridPrecision);
						if (!(good = ((*i)->selected() && (*i)->selected()->rowIsValid(r))))
							break;
					}
					if (good)
					{
						pGridHelper->indicesRows[last] = pGridHelper->indicesRows[r];
						++last;
					}
				}
				pGridHelper->virtualSize.y = last;
			}

			if (!onCols.empty())
			{
				int last = 0; /* The count of valid cols */
				const InputVector::iterator end = onCols.end();
				bool good;
				for (int c = 0; c != pGridHelper->virtualSize.x; ++c)
				{
					good = true;
					for (InputVector::iterator i = onCols.begin(); i != end; ++i)
					{
						(*i)->selected()->dataGridPrecision(pDataGridPrecision);
						if (!(good = ((*i)->selected() && (*i)->selected()->colIsValid(c))))
							break;
					}
					if (good)
					{
						pGridHelper->indicesCols[last] = pGridHelper->indicesCols[c];
						++last;
					}
				}
				pGridHelper->virtualSize.x = last;
			}

			if (!onCells.empty())
			{
			}

		}

		// Invalidating the cache of the wxGrid
		wxGridTableBase* tbl = pGrid->GetTable();
		if (tbl)
			pGrid->SetTable(tbl, false);
		pGrid->EndBatch();

		
	}





} // namespace Filter
} // namespace Toolbox
} // namespace Antares

