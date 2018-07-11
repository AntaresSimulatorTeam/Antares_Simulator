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

#include "edit.h"
#include "../../application/study.h"
#include "../../toolbox/create.h"
#include <wx/window.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/stattext.h>
#include <antares/utils.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/choice.h>



using namespace Yuni;


namespace Antares
{
namespace Window
{


	BindingConstraintInfoEditor::BindingConstraintInfoEditor(wxWindow* parent, Data::BindingConstraint* constraint) :
		wxDialog(parent, wxID_ANY, wxT("Binding constraint"), wxDefaultPosition, wxDefaultSize,
			wxCLOSE_BOX|wxCAPTION|wxSYSTEM_MENU|wxCLIP_CHILDREN),
		pConstraint(constraint),
		pType(nullptr),
		pOperator(nullptr)
	{
		wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		wxFlexGridSizer* pGridSizer = new wxFlexGridSizer(2, 0, 0);
		pGridSizer->AddGrowableCol(1);

		// Caption
		{
			pGridSizer->Add(Component::CreateLabel(this, wxT("Caption "), true), 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			wxString s;
			if (pConstraint)
				s = wxStringFromUTF8(pConstraint->name());
			else
				s = wxT("New binding constraint");
			pCaption = new wxTextCtrl(this, wxID_ANY, s, wxDefaultPosition, wxDefaultSize);
			pGridSizer->Add(pCaption, 1, wxALL| wxEXPAND, 1);
		}

		{
			pGridSizer->Add(Component::CreateLabel(this, wxT("     Comments ")),
				0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			wxString s;
			if (pConstraint)
				s = wxStringFromUTF8(pConstraint->comments());
			pComments = new wxTextCtrl(this, wxID_ANY, s, wxDefaultPosition, wxDefaultSize);
			pGridSizer->Add(pComments, 1, wxALL| wxEXPAND, 1);
		}

		// Type of the constraint
		if (!pConstraint or pConstraint->type() != Data::BindingConstraint::typeHourly)
		{
			pGridSizer->Add(Component::CreateLabel(this, wxT("Type ")),
				0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			pType = new wxChoice(this, wxID_ANY);
			wxArrayString types;
			if (!pConstraint)
				types.Add(wxT("Hourly"));
			types.Add(wxT("Daily"));
			types.Add(wxT("Weekly"));
			pType->Append(types);
			if (!pConstraint)
				pType->SetSelection(0);
			else
			{
				switch (pConstraint->type())
				{
					case Data::BindingConstraint::typeDaily:
						pType->SetSelection(0);
						break;
					case Data::BindingConstraint::typeWeekly:
						pType->SetSelection(1);
						break;
					case Data::BindingConstraint::typeHourly:
					case Data::BindingConstraint::typeMax:
					case Data::BindingConstraint::typeUnknown:
						assert(false);
						break;
				}
			}

			wxBoxSizer* splitter = new wxBoxSizer(wxHORIZONTAL);
			splitter->Add(pType, 2, wxALL|wxEXPAND);
			splitter->Add(5, 5, 3);
			pGridSizer->Add(splitter, 0, wxALL| wxEXPAND, 1);
		}

		// Bounds
		{
			auto* t = new wxStaticText(this, wxID_ANY,
				wxT("Bounds "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			pOperator = new wxChoice(this, wxID_ANY);
			wxArrayString types;
			types.Add(wxT("<"));
			types.Add(wxT(">"));
			types.Add(wxT("="));
			types.Add(wxT("< and >"));
			pOperator->Append(types);

			if (pConstraint)
			{
				switch (pConstraint->operatorType())
				{
					case Data::BindingConstraint::opLess:
						pOperator->SetSelection(0);
						break;
					case Data::BindingConstraint::opGreater:
						pOperator->SetSelection(1);
						break;
					case Data::BindingConstraint::opEquality:
						pOperator->SetSelection(2);
						break;
					case Data::BindingConstraint::opBoth:
						pOperator->SetSelection(3);
						break;
					case Data::BindingConstraint::opUnknown:
						pOperator->SetSelection(0);
						break;
					case Data::BindingConstraint::opMax:
						pOperator->SetSelection(0);
						break;
				}
			}
			else
				pOperator->SetSelection(0);

			wxBoxSizer* splitter = new wxBoxSizer(wxHORIZONTAL);
			splitter->Add(pOperator, 2, wxALL|wxEXPAND);
			splitter->Add(5, 5, 3);
			pGridSizer->Add(splitter, 0, wxALL| wxEXPAND, 1);
		}

		// Enable / Disable
		{
			auto* t = new wxStaticText(this, wxID_ANY,
				wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			pGridSizer->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);

			pEnabled = new wxCheckBox(this, wxID_ANY, wxT("enabled"));
			if (pConstraint)
				pEnabled->SetValue(pConstraint->enabled());
			else
				pEnabled->SetValue(true);
			wxSizer* vs = new wxBoxSizer(wxVERTICAL);
			vs->AddSpacer(8);
			vs->Add(pEnabled, 0, wxALL|wxEXPAND, 1);

			wxBoxSizer* splitter = new wxBoxSizer(wxHORIZONTAL);
			splitter->Add(vs, 2, wxALL|wxEXPAND);
			splitter->Add(5, 5, 3);
			pGridSizer->Add(splitter, 0, wxALL| wxEXPAND, 1);
		}

		sizer->Add(pGridSizer, 1, wxALL|wxEXPAND, 20);


		sizer->AddSpacer(13);
		wxBoxSizer* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
		pnlBtns->AddStretchSpacer();

		wxButton* btn;

		// Cancel
		btn = Component::CreateButton(this, wxT("Cancel"), this, &BindingConstraintInfoEditor::onCancel);
		pnlBtns->Add(btn);
		pnlBtns->AddSpacer(4);

		// Save !
		btn = Component::CreateButton(this,
			((pConstraint) ? wxT("Save and Close") : wxT("Create the binding constraint")),
			this, &BindingConstraintInfoEditor::onSave);
		btn->SetDefault();
		pnlBtns->Add(btn);


		pnlBtns->AddSpacer(20);
		sizer->Add(pnlBtns, 0, wxALL|wxEXPAND);
		sizer->AddSpacer(10);

		sizer->Layout();
		SetSizer(sizer);

		GetSizer()->Fit(this);
		wxSize p = GetSize();
		p.SetWidth(p.GetWidth() + 20);
		if (p.GetWidth() < 450)
			p.SetWidth(450);
		SetSize(p);
		Centre(wxBOTH);
	}



	void BindingConstraintInfoEditor::onCancel(void*)
	{
		Dispatcher::GUI::Close(this);
	}


	void BindingConstraintInfoEditor::onSave(void*)
	{
		auto studyptr = Data::Study::Current::Get();
		if (!studyptr)
			return;

		auto& study = *studyptr;

		// assert
		assert(study.uiinfo);
		assert(pCaption);
		assert(pComments);

		// Avoid useless and hazardous refresh
		StudyUpdateLocker studylocker;

		// The new name for the constraint
		String newname;
		{
			String edittext;
			wxStringToString(pCaption->GetValue(), edittext);
			BeautifyName(newname, edittext);
			if (newname.empty())
			{
				logs.error() << "You must specify a name for the constraint";
				return;
			}
		}

		// A temporary string, for conversion
		String tmp;

		if (pConstraint)
		{
			if (not newname.empty() and pConstraint->name() != newname)
			{
				if (not study.bindingConstraints.rename(pConstraint, newname))
				{
					logs.error() << "Impossible to rename the binding constraint \""
						<< pConstraint->name() << "\" into \"" << newname << "\".";
					return;
				}

				// mark all values as modified
				pConstraint->markAsModified();
			}

			logs.info() << "saving changes for the constraint " << pConstraint->name();
			// Comments
			{
				const wxString& wxcomments = pComments->GetValue();
				wxStringToString(wxcomments, tmp);
				pConstraint->comments(tmp);
			}
			// Enabled
			{
				assert(pEnabled);
				pConstraint->enabled(pEnabled->GetValue());
			}
			// Operator
			{
				assert(pOperator);
				wxStringToString(pOperator->GetStringSelection(), tmp);
				pConstraint->operatorType(Data::BindingConstraint::StringToOperator(tmp));
				assert(pConstraint->operatorType() != Data::BindingConstraint::opUnknown);
			}
			// Type
			if (pConstraint->type() != Data::BindingConstraint::typeHourly)
			{
				assert(pType);
				wxStringToString(pType->GetStringSelection(), tmp);
				pConstraint->mutateTypeWithoutCheck(Data::BindingConstraint::StringToType(tmp));
				assert(pConstraint->type() != Data::BindingConstraint::typeUnknown);
			}

			// Reload runtime data
			study.uiinfo->reloadBindingConstraints();
			OnStudyConstraintModified(pConstraint);
		}
		else
		{
			String id;
			TransformNameIntoID(newname, id);
			if (study.bindingConstraints.findByName(newname) or study.bindingConstraints.find(id))
			{
				logs.error() << "A binding constraint with this name already exists.";
				return;
			}
			auto* constraint = study.bindingConstraints.add(newname);
			if (!constraint)
			{
				logs.error() << "Impossible to add a new binding constraint";
				return;
			}
			logs.info() << "creating new constraint " << newname;

			// mark all values as modified
			constraint->markAsModified();

			wxStringToString(pOperator->GetStringSelection(), tmp);
			const Data::BindingConstraint::Operator op = Data::BindingConstraint::StringToOperator(tmp);
			assert(op != Data::BindingConstraint::opUnknown);

			wxStringToString(pType->GetStringSelection(), tmp);
			const Data::BindingConstraint::Type t =	Data::BindingConstraint::StringToType(tmp);
			assert(t != Data::BindingConstraint::typeUnknown);

			// Reseting
			constraint->clearAndReset(newname, t, op);
			constraint->enabled(pEnabled->GetValue());

			// Comments
			{
				wxStringToString(pComments->GetValue(), tmp);
				constraint->comments(tmp);
			}

			constraint->setAllWeightedLinksOnLayer(study.activeLayerID);
			// mark all values as modified
			constraint->markAsModified();

			// Reload runtime data
			study.uiinfo->reloadBindingConstraints();
			OnStudyConstraintAdded(constraint);
		}

		// Disable the window
		Enable(false);
		// The study has been modified
		MarkTheStudyAsModified();

		// Close the window
		Dispatcher::GUI::Close(this);
	}





} // namespace Window
} // namespace Antares

