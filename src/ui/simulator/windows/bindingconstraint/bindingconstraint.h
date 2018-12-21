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
#ifndef __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__
# define __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__

# include <antares/wx-wrapper.h>
# include "../../toolbox/components/notebook/notebook.h"
# include "../../toolbox/components/datagrid/component.h"
# include "../../toolbox/input/bindingconstraint.h"
# include <ui/common/component/panel.h>



namespace Antares
{
namespace Window
{

	class BindingConstraintDataPanel;
	class BindingConstraintWeightsPanel;
	class BindingConstraintOffsetsPanel;



	/*!
	** \brief Settings for a single Binding constraint
	*/
	class BindingConstraint : public Component::Panel, public Yuni::IEventObserver<BindingConstraint>
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param parent The parent window
		*/
		BindingConstraint(wxWindow* parent);
		//! Destructor
		virtual ~BindingConstraint();
		//@}

		void selectDefaultPage();

	private:
		void onPageChanged(Component::Notebook::Page& page);

		void onSelectedConstraint(Data::BindingConstraint* contraint);
		void onDblClickConstraint(Data::BindingConstraint* contraint);

		void onAdd(void*);
		void onAddFromMouse(wxMouseEvent&);
		void onDelete(void*);
		void onDeleteFromMouse(wxMouseEvent&);
		void onEdit(void*);
		void onDeleteAll(void*);
		void onEditFromMouse(wxMouseEvent&);
		void onBuild(void*);
		
	private:
		Component::Notebook::Page* pPageList;
		Component::Notebook::Page* pPageWeights;
		Component::Notebook::Page* pPageOffsets;
		Component::Notebook::Page* pPageEqual;
		Component::Notebook::Page* pPageLess;
		Component::Notebook::Page* pPageGreater;
		Toolbox::InputSelector::BindingConstraint* pAllConstraints;
		Data::BindingConstraint* pSelected;

		BindingConstraintDataPanel* pDataPanelEqual;
		BindingConstraintDataPanel* pDataPanelLess;
		BindingConstraintDataPanel* pDataPanelGreater;
		BindingConstraintWeightsPanel* pWeightsPanel;
		BindingConstraintOffsetsPanel* pOffsetsPanel;

	public:
		// Event table
		DECLARE_EVENT_TABLE()

	}; // class BindingConstraint







} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_H__
