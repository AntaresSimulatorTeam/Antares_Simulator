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
#ifndef __ANTARES_TOOLBOX_FILTER_COMPONENT_H__
# define __ANTARES_TOOLBOX_FILTER_COMPONENT_H__

# include <antares/wx-wrapper.h>
# include <wx/panel.h>
# include <vector>
# include <list>
# include "input.h"
# include "../gridhelper.h"
# include "filter.h"
# include <antares/date.h>
# include "panel.h"



namespace Antares
{
namespace Toolbox
{
namespace Filter
{


	class Component : public Antares::Component::Panel
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		** \param parent The parent window
		*/
		Component(wxWindow* parent, Date::Precision precision = Date::stepNone);
		//! Destructor
		virtual ~Component();
		//@}

		Date::Precision precision() const {return pPrecision;}
		void precision(const Date::Precision p) {pPrecision = p;}

		/*!
		** \brief Create and add an input
		*/
		Input* add();

		/*!
		** \brief Remove an input
		*/
		bool remove(Input* in);
		void remove(int id);

		/*!
		** \brief Remove all inputs
		*/
		void clear();

		void updateSearchResults();

		void refresh();

		Yuni::Event<void()> onUpdateSearchResults;

		//! \name Grid
		//@{
		wxGrid* grid() const {return pGrid;}
		void grid(wxGrid* g) {pGrid = g;}

		Antares::Component::Datagrid::VGridHelper* gridHelper() const {return pGridHelper;}
		void gridHelper(Antares::Component::Datagrid::VGridHelper* g) {pGridHelper = g;}
		//@}

		/*!
		** \brief Set the precision required by the datagrid
		*/
		void dataGridPrecision(Date::Precision p) {pDataGridPrecision = p;}

	private:
		//! Input List
		typedef std::list<Input*> InputList;
		typedef std::vector<Input*> InputVector;

		void evtRefreshGrid();

		void classifyFilters(InputVector& onRows, InputVector& onCols, InputVector& onCells);

	private:
		//! All inputs
		InputList pInputs;
		//! Attached grid
		wxGrid* pGrid;
		//! Grid Helper
		Antares::Component::Datagrid::VGridHelper* pGridHelper;

		//! Concurrent refresh count
		int pRefreshBatchCount;

		//! Precision
		Date::Precision pPrecision;
		//!
		Date::Precision pDataGridPrecision;

	}; // class Component






} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_COMPONENT_H__
