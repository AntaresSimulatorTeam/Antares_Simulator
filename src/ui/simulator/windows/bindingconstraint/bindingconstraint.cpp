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

#include "bindingconstraint.h"

#include "../../toolbox/validator.h"

#include <ui/common/component/panel.h>
#include "../../toolbox/components/datagrid/component.h"
#include "../../toolbox/components/button.h"
#include "../../toolbox/components/datagrid/gridhelper.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/weights.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/offsets.h"
#include "../../toolbox/components/datagrid/renderer/bindingconstraint/data.h"
#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../windows/message.h"

#include "edit.h"
#include "../../application/main.h"
#include "../../application/wait.h"


using namespace Yuni;


namespace Antares
{
namespace Window
{


	class BindingConstraintDataPanel final :
		public Component::Panel,
		public Yuni::IEventObserver<BindingConstraintDataPanel>
	{
	public:
		BindingConstraintDataPanel(wxWindow* parent, Data::BindingConstraint::Operator op) :
			Component::Panel(parent),
			pOperator(op)
		{
			auto* sizer = new wxBoxSizer(wxVERTICAL);

			auto* notebook = new Component::Notebook(this, Component::Notebook::orTop);
			notebook->theme(Component::Notebook::themeLight);

			// The pseudo panel should have their size non-null for proper alignment
			pPageHourly  = notebook->add(new wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxSize(2, 2)), wxT(" Hourly "));
			pPageDaily   = notebook->add(new wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxSize(2, 2)), wxT(" Daily "));
			pPageWeekly  = notebook->add(new wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxSize(2, 2)), wxT(" Weekly "));
			sizer->Add(notebook, 0, wxALL|wxEXPAND);

			typedef Component::Datagrid::Renderer::BindingConstraint::Data RendererType;
			auto* renderer = new RendererType(this, pOperator);
			onBindingConstraintTypeChanged.connect(renderer, &RendererType::bindingConstraintTypeChanged);
			onBindingConstraintTypeChanged.connect(this, &BindingConstraintDataPanel::bindingConstraintTypeChanged);
			grid = new Component::Datagrid::Component(this, renderer, wxEmptyString, true, true,
				false, false, true);
			sizer->Add(grid, 1, wxALL|wxEXPAND);

			SetSizer(sizer);
			pPageHourly->select();

			notebook->onPageChanged.connect(this, &BindingConstraintDataPanel::onPageChanged);
		}

		virtual ~BindingConstraintDataPanel()
		{
			destroyBoundEvents();
		}

		void forceRefresh()
		{
			assert(grid && "Invalid pointer to grid");
			assert(GetSizer() && "Invalid sizer");

			if (Data::Study::Current::Valid())
			{
				grid->InvalidateBestSize();
				grid->forceRefresh();
				Data::UIRuntimeInfo& uiinfo = *(Data::Study::Current::Get()->uiinfo);
				wxString s;
				uint c;

				c = uiinfo.countItems(pOperator, Data::BindingConstraint::typeHourly);
				s << wxT("  Hourly  (") << c << wxT(") ");
				pPageHourly->caption(s);

				c = uiinfo.countItems(pOperator, Data::BindingConstraint::typeDaily);
				s.clear();
				s << wxT("  Daily  (") << c << wxT(") ");
				pPageDaily->caption(s);

				c = uiinfo.countItems(pOperator, Data::BindingConstraint::typeWeekly);
				s.clear();
				s << wxT("  Weekly  (") << c << wxT(") ");
				pPageWeekly->caption(s);

				GetSizer()->Layout();
			}
		}

		void selectDefaultPage()
		{
			// Select the first page
			pPageHourly->select(true);
			// Making sure that the event has been done, even on Windows...
			bindingConstraintTypeChanged(Data::BindingConstraint::typeHourly);
		}

	public:
		Component::Datagrid::Component* grid;

	private:
		void onPageChanged(Component::Notebook::Page& page)
		{
			if (&page == pPageHourly)
				onBindingConstraintTypeChanged(Data::BindingConstraint::typeHourly);
			else
			{
				if (&page == pPageDaily)
					onBindingConstraintTypeChanged(Data::BindingConstraint::typeDaily);
				else
					onBindingConstraintTypeChanged(Data::BindingConstraint::typeWeekly);
			}
			grid->forceRefresh();
		}

		void bindingConstraintTypeChanged(Data::BindingConstraint::Type type)
		{
			if (type == Data::BindingConstraint::typeHourly)
			{
				pPageHourly->select(true);
				return;
			}
			if (type == Data::BindingConstraint::typeDaily)
			{
				pPageDaily->select(true);
				return;
			}
			if (type == Data::BindingConstraint::typeWeekly)
			{
				pPageWeekly->select(true);
				return;
			}
			logs.warning() << "internal error: invalid type for selecting binding constraint type";
		}

	private:
		const Data::BindingConstraint::Operator pOperator;
		static Yuni::Event<void (Data::BindingConstraint::Type)> onBindingConstraintTypeChanged;
		Component::Notebook::Page* pPageHourly;
		Component::Notebook::Page* pPageDaily;
		Component::Notebook::Page* pPageWeekly;

	}; // class BindingConstraintDataPanel




	Yuni::Event<void (Data::BindingConstraint::Type)> BindingConstraintDataPanel::onBindingConstraintTypeChanged;


	BEGIN_EVENT_TABLE(BindingConstraint, wxPanel)
	END_EVENT_TABLE()










	BindingConstraint::BindingConstraint(wxWindow* parent) :
		Panel(parent),
		pPageList(nullptr),
		pPageWeights(nullptr),
		pPageOffsets(nullptr),
		pPageEqual(nullptr),
		pPageLess(nullptr),
		pPageGreater(nullptr),
		pGridWeights(nullptr),
		pAllConstraints(nullptr),
		pSelected(nullptr),
		pDataPanelEqual(nullptr),
		pDataPanelLess(nullptr),
		pDataPanelGreater(nullptr)
	{
		// Main sizer
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		auto* n = new Component::Notebook(this, Component::Notebook::orTop);
		n->caption(wxT("Binding constraints"));

		// List of all binding constraints
		{
			wxPanel* p = new wxPanel(n, wxID_ANY);
			p->SetSizer(new wxBoxSizer(wxVERTICAL));
			wxSizer* h = new wxBoxSizer(wxHORIZONTAL);

			h->AddSpacer(10);

			Component::Button* btn;

			// Create a new thermal constraint
			btn = new Component::Button(p, wxT("Create a constraint"), "images/16x16/constraint_plus.png",
				this, &BindingConstraint::onAdd);
			h->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);

			// Delete
			btn = new Component::Button(p, wxT("Delete"), "images/16x16/constraint_minus.png",
				this, &BindingConstraint::onDelete);
			h->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);

			// vertical line
			Component::AddVerticalSeparator(p, h);

			// Edit
			btn = new Component::Button(p, wxT("Edit"), "images/16x16/constraint_edit.png",
				this, &BindingConstraint::onEdit);
			h->Add(btn, 0, wxALIGN_CENTER_VERTICAL|wxALL);

			// vertical line
			Component::AddVerticalSeparator(p, h);

			// Delete  All
			btn = new Component::Button(p, wxT("Delete all"), "images/16x16/delete_all_BC.png",
				this, &BindingConstraint::onDeleteAll);
			h->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);


			p->GetSizer()->Add(h, 0, wxALL|wxEXPAND, 2);

			pAllConstraints = new Toolbox::InputSelector::BindingConstraint(p);
			p->GetSizer()->Add(pAllConstraints, 1, wxALL|wxEXPAND);
			pPageList = n->add(p, wxT("list"), wxT("  Summary  "));
		}

		// Weights
		{
			pGridWeights = new Component::Datagrid::Component(n,
				new Component::Datagrid::Renderer::BindingConstraint::Weights(n), wxEmptyString, true, true,
				false,false, true);
			pPageWeights = n->add(pGridWeights, wxT("weights"), wxT("  Weights  "));
		}

		// Offsets
		{
			pGridOffsets = new Component::Datagrid::Component(n,
				new Component::Datagrid::Renderer::BindingConstraint::Offsets(n), wxEmptyString, true, true,
				false, false, true);
			pPageOffsets = n->add(pGridOffsets, wxT("offsets"), wxT("  Offsets  "));
		}

		// Second membre
		{
			BindingConstraintDataPanel* p;

			p = new BindingConstraintDataPanel(n, Data::BindingConstraint::opEquality);
			pPageEqual = n->add(p, wxT("equality"),  wxT("    =    "));
			pDataPanelEqual = p;

			p = new BindingConstraintDataPanel(n, Data::BindingConstraint::opLess);
			pPageLess = n->add(p,  wxT("less"),     wxT("    <    "));
			pDataPanelLess = p;

			p = new BindingConstraintDataPanel(n, Data::BindingConstraint::opGreater);
			pPageGreater = n->add(p,  wxT("greater"),  wxT("    >    "));
			pDataPanelGreater = p;
		}

		// Post
		n->select(wxT("list"));
		sizer->Add(n, 1, wxALL|wxEXPAND);

		// Events
		n->onPageChanged.connect(this, &BindingConstraint::onPageChanged);

		// Update the layout
		SetSizer(sizer);
		GetSizer()->Layout();

		pAllConstraints->onBindingConstraintChanged.connect(this, &BindingConstraint::onSelectedConstraint);
		pAllConstraints->onBindingConstraintDblClick.connect(this, &BindingConstraint::onDblClickConstraint);
	}


	BindingConstraint::~BindingConstraint()
	{
		pSelected = nullptr;
		destroyBoundEvents();

		// To avoid corrupt vtable in some rare cases / misuses
		// (when children try to access to this object for example),
		// we should destroy all children as soon as possible.
		auto* sizer = GetSizer();
		if (sizer)
			sizer->Clear(true);
	}


	void BindingConstraint::onPageChanged(Component::Notebook::Page& page)
	{
		InvalidateBestSize();
		Refresh();

		if (&page == pPageList)
		{
			logs.info() << "notebook binding constraints: page changed: all constraints";
			pAllConstraints->InvalidateBestSize();
			pAllConstraints->updateRowContent();
			pAllConstraints->Refresh();
			return;
		}
		if (&page == pPageWeights)
		{
			logs.info() << "notebook binding constraints: page changed: weights";
			pGridWeights->InvalidateBestSize();
			pGridWeights->forceRefresh();
			pGridWeights->Refresh();
			return;
		}
		if (&page == pPageOffsets)
		{
			logs.info() << "notebook binding constraints: page changed: offsets";
			pGridOffsets->InvalidateBestSize();
			pGridOffsets->forceRefresh();
			pGridOffsets->Refresh();
			return;
		}
		if (&page == pPageLess)
		{
			logs.info() << "notebook binding constraints: page changed: less";
			pDataPanelLess->forceRefresh();
			return;
		}
		if (&page == pPageGreater)
		{
			logs.info() << "notebook binding constraints: page changed: greater";
			pDataPanelGreater->forceRefresh();
			return;
		}
		if (&page == pPageEqual)
		{
			logs.info() << "notebook binding constraints: page changed: equal";
			pDataPanelEqual->forceRefresh();
			return;
		}
	}


	void BindingConstraint::onAddFromMouse(wxMouseEvent& evt)
	{
		onAdd(nullptr);
		evt.Skip();
	}


	void BindingConstraint::onDeleteFromMouse(wxMouseEvent& evt)
	{
		onDelete(nullptr);
		evt.Skip();
	}


	void BindingConstraint::onEditFromMouse(wxMouseEvent& evt)
	{
		onEdit(nullptr);
		evt.Skip();
	}


	void BindingConstraint::onAdd(void*)
	{
		auto studyptr = Data::Study::Current::Get();
		if (not studyptr)
			return;
		auto& study = *studyptr;

		// To avoid some useless refresh (and flickering in the same time)
		// We will keep the current number of binding constraints and we
		// will compare it after the deletion of the window to make a refresh
		// if required.
		uint oldCount = study.bindingConstraints.size();

		// The Window
		auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), nullptr);
		editor->ShowModal();
		editor->Destroy();
		WIP::Locker wip;
		// Refresh only if required
		if (oldCount != study.bindingConstraints.size())
		{
			InvalidateBestSize();
			pAllConstraints->update();
			pAllConstraints->InvalidateBestSize();
			pAllConstraints->updateRowContent();
			Dispatcher::GUI::Refresh(pAllConstraints);

			MarkTheStudyAsModified();
			Dispatcher::GUI::Refresh(this);
		}
	}


	void BindingConstraint::onDelete(void*)
	{
		if (not pSelected)
			return;
		auto studyptr = Data::Study::Current::Get();
		if (not studyptr)
			return;
		auto& study = *studyptr;

		auto& mainFrm = *Forms::ApplWnd::Instance();

		Window::Message message(&mainFrm, wxT("Binding constraint"),
			wxT("Delete a binding constraint"),
			wxString() << wxT("Do you really want to delete the binding constraint '")
			<< wxStringFromUTF8(pSelected->name()) << wxT("' ?")
			);
		message.add(Window::Message::btnYes);
		message.add(Window::Message::btnCancel, true);
		if (message.showModal() == Window::Message::btnYes)
		{
			logs.info() << "deleting the constraint " << pSelected->name();
			WIP::Locker wip;
			study.bindingConstraints.remove(pSelected);
			study.uiinfo->reloadBindingConstraints();
			pSelected = nullptr;

			InvalidateBestSize();
			pAllConstraints->update();
			pAllConstraints->InvalidateBestSize();
			pAllConstraints->updateRowContent();
			OnStudyConstraintDelete(nullptr);
			Dispatcher::GUI::Refresh(pAllConstraints);

			MarkTheStudyAsModified();
			Dispatcher::GUI::Refresh(this);
		}
	}


	void BindingConstraint::onEdit(void*)
	{
		if (not pSelected)
			return;
		auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), pSelected);
		editor->ShowModal();
		editor->Destroy();
		pAllConstraints->updateRowContent();
	}

	void BindingConstraint::onDeleteAll(void *)
	{

		auto studyptr = Data::Study::Current::Get();
		if (not studyptr)
			return;
		auto& study = *studyptr;

		auto& mainFrm = *Forms::ApplWnd::Instance();

		Window::Message message(&mainFrm, wxT("Binding constraint"),
			wxT("Delete all binding constraints"),
			wxString() << wxT("Do you really want to delete all the binding constraints ")
			<< wxT("' ?")
		);
		message.add(Window::Message::btnYes);
		message.add(Window::Message::btnCancel, true);
		if (message.showModal() == Window::Message::btnYes)
		{
			logs.info() << "deleting the constraints ";
			WIP::Locker wip;
			
			Antares::Data::BindConstList::iterator BC = study.bindingConstraints.begin();
			int BCListSize = study.bindingConstraints.size();
			for( int i = 0; i< BCListSize; i++)
			{

				if ((*BC)->hasAllWeightedLinksOnLayer(study.activeLayerID))
					study.bindingConstraints.remove(*BC);
				else
					++BC;

			}

			study.uiinfo->reloadBindingConstraints();
			pSelected = nullptr;

			InvalidateBestSize();
			pAllConstraints->update();
			pAllConstraints->InvalidateBestSize();
			pAllConstraints->updateRowContent();
			OnStudyConstraintDelete(nullptr);
			Dispatcher::GUI::Refresh(pAllConstraints);

			MarkTheStudyAsModified();
			Dispatcher::GUI::Refresh(this);
		}
	}


	void BindingConstraint::onSelectedConstraint(Data::BindingConstraint* constraint)
	{
		pSelected = (Data::Study::Current::Valid() ? constraint : nullptr);
	}


	void BindingConstraint::onDblClickConstraint(Data::BindingConstraint* constraint)
	{
		pSelected = (Data::Study::Current::Valid() ? constraint : nullptr);
		auto* editor = new BindingConstraintInfoEditor(Forms::ApplWnd::Instance(), pSelected);
		editor->ShowModal();
		editor->Destroy();
		pAllConstraints->updateRowContent();
	}


	void BindingConstraint::selectDefaultPage()
	{
		if (pPageList)
			pPageList->select();
	}





} // namespace Window
} // namespace Antares

