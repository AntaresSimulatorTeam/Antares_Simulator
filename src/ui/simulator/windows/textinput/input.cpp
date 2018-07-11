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
#include <antares/study/constraint.h>
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../toolbox/components/wizardheader.h"
#include "../../toolbox/create.h"
#include <antares/utils.h>

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/window.h>
#include <wx/stattext.h>


using namespace Yuni;


namespace Antares
{
namespace Window
{


	namespace // anonymous
	{

		class ValueDialog final : public wxDialog
		{
		public:
			//! \name Constructor & Destructor
			//@{
			/*!
			** \brief Constructor
			*/
			ValueDialog(wxWindow* parent, const wxString& out, const wxString& title, const wxString& caption);
			//! Destructor
			virtual ~ValueDialog() {}
			//@}

		public:
			bool canceled;
			wxString value;

		private:
			void onCancel(void*);
			void onPerform(void*);

		private:
			wxFlexGridSizer* pGridSizer;
			wxTextCtrl* pTextInput;

		}; // class ValueDialog



		//! The previous value entered by the user (persistence)
		static wxString gPreviousValue;



		ValueDialog::ValueDialog(wxWindow* parent, const wxString& out, const wxString& title, const wxString& caption) :
			wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
				wxCLOSE_BOX|wxCAPTION|wxSYSTEM_MENU|wxCLIP_CHILDREN)
		{
			canceled = true;

			auto* sizer = new wxBoxSizer(wxVERTICAL);

			sizer->Add(Toolbox::Components::WizardHeader::Create(this,
					title,
					"images/32x32/resize-matrix.png",
					wxEmptyString,
					200), 0, wxALL|wxEXPAND);

			sizer->AddSpacer(10);

			pGridSizer = new wxFlexGridSizer(2, 0, 0);
			pGridSizer->AddGrowableCol(1);

			{
				wxStaticText* t = Component::CreateLabel(this, caption, true);
				wxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
				hz->Add(t, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				hz->AddSpacer(5);
				pGridSizer->Add(hz, 0, wxALL|wxEXPAND);

				wxString s;
				if (gPreviousValue.empty() || !out.empty())
					s << out;
				else
					s << gPreviousValue;
				pTextInput = new wxTextCtrl(this, wxID_ANY, s, wxDefaultPosition, wxDefaultSize);
				pGridSizer->Add(pTextInput, 1, wxALL| wxEXPAND, 1);
			}

			sizer->Add(pGridSizer, 1, wxALL|wxEXPAND, 20);


			sizer->AddSpacer(25);

			wxBoxSizer* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
			pnlBtns->AddStretchSpacer();

			// Cancel
			auto* pBtnCancel = Component::CreateButton(this, wxT("Cancel"), this, &ValueDialog::onCancel);
			pnlBtns->Add(pBtnCancel);
			pnlBtns->AddSpacer(5);

			// Gogogo !
			auto* pBtnSave = Component::CreateButton(this, wxT("Continue"), this, &ValueDialog::onPerform);
			pnlBtns->Add(pBtnSave);
			pBtnSave->SetDefault();

			pnlBtns->AddSpacer(20);
			sizer->Add(pnlBtns, 0, wxALL|wxEXPAND);
			sizer->AddSpacer(10);

			sizer->Layout();

			SetSizer(sizer);

			GetSizer()->Fit(this);
			wxSize p = GetSize();
			p.SetWidth(380);
			SetSize(p);
			Centre(wxBOTH);

			pTextInput->SetSelection(-1, -1);
			pTextInput->SetFocus();
		}


		void ValueDialog::onCancel(void*)
		{
			value.clear();
			Dispatcher::GUI::Close(this);
		}


		void ValueDialog::onPerform(void*)
		{
			assert(pTextInput);

			value = pTextInput->GetValue();
			gPreviousValue = value;
			canceled = false;
			Dispatcher::GUI::Close(this);
		}

	} // anonymous namespace







	bool AskForInput(wxString& out, const wxString& title, const wxString& caption)
	{
		ValueDialog dlg(Antares::Forms::ApplWnd::Instance(), out, title, caption);
		dlg.ShowModal();
		if (not dlg.canceled)
		{
			out = dlg.value;
			return true;
		}
		return false;
	}





} // namespace Window
} // namespace Antares
