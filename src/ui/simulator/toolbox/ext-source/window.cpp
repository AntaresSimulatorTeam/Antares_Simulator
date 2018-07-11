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
#include "window.h"

#include "../../toolbox/resources.h"
#include "../../toolbox/create.h"
#include "../../toolbox/components/notebook/notebook.h"

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/scrolwin.h>
#include <wx/dcbuffer.h>
#include "action-panel.h"

#include "performer.h"



using namespace Yuni;



namespace Antares
{
namespace Window
{

	namespace // anonymous
	{

		class ActionsScrollWindow : public wxScrolledWindow
		{
		public:
			explicit ActionsScrollWindow(wxWindow* parent)
				:wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
					wxVSCROLL)
			{
				SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
			}

			virtual ~ActionsScrollWindow()
			{
			}

		private:
			void onDraw(wxPaintEvent&);
			void onChildFocus(wxChildFocusEvent& evt)
			{
				evt.Skip();
			}
			DECLARE_EVENT_TABLE()
		};

		BEGIN_EVENT_TABLE(ActionsScrollWindow, wxScrolledWindow)
			EVT_PAINT(ActionsScrollWindow::onDraw)
			EVT_CHILD_FOCUS(ActionsScrollWindow::onChildFocus)
		END_EVENT_TABLE()

		void ActionsScrollWindow::onDraw(wxPaintEvent&)
		{
			// The DC
			wxAutoBufferedPaintDC dc(this);
			if (!dc.IsOk())
				return;
			// Shifts the device origin so we don't have to worry
			// about the current scroll position ourselves
			PrepareDC(dc);
			if (!dc.IsOk())
				return;

			typedef Antares::Private::Window::ActionPanel ActionPanel;
			ActionPanel::DrawBackgroundWithoutItems(*this, dc, GetClientSize());
		}

	} // anonymous namespace





	ApplyActionsDialog::ApplyActionsDialog(wxWindow* parent, const Antares::Action::Context::Ptr& context,
			const Antares::Action::IAction::Ptr& root)
		:wxDialog(parent, wxID_ANY, wxT("Import"), wxDefaultPosition, wxDefaultSize),
		pContext(context),
		pActions(root)
	{
		wxColour defaultBgColor = GetBackgroundColour();
		//SetBackgroundColour(wxColour(255, 255, 255));

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddSpacer(10);

		auto* pNotebook = new Component::Notebook(this);
		pNotebook->theme(Component::Notebook::themeLight);
		sizer->Add(pNotebook, 1, wxALL|wxEXPAND);


		auto* panelScroll = new Component::Panel(pNotebook);
		{
			wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
			ActionsScrollWindow* vscroll = new ActionsScrollWindow(panelScroll);
			vscroll->SetScrollRate(5, 5);
			wxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);
			vscroll->SetSizer(scrollSizer);
			panelSizer->AddSpacer(5);
			panelSizer->Add(vscroll, 1, wxALL|wxEXPAND);
			panelSizer->AddSpacer(15);
			panelScroll->SetSizer(panelSizer);

			// Add the first node
			typedef Antares::Private::Window::ActionPanel ActionPanel;
			vscroll->Freeze();

			// Register all views
			root->registerViews(*pContext);

			ActionPanel* firstAction = new ActionPanel(vscroll, nullptr, pContext, root);
			scrollSizer->Add(firstAction, 0, wxALL|wxEXPAND, 0);
			vscroll->Thaw();
		}


		Component::Panel* panel = new Component::Panel(this);
		wxBoxSizer* sizerBar = new wxBoxSizer(wxHORIZONTAL);
		pLblInfos = Component::CreateLabel(panel, wxEmptyString, false, true);
		sizerBar->Add(20, 5);
		sizerBar->Add(pLblInfos, 0, wxALL|wxALIGN_CENTER_VERTICAL);
		sizerBar->AddStretchSpacer();
		panel->SetSizer(sizerBar);
		panel->SetBackgroundColour(defaultBgColor);

		// Button Close
		{
			wxButton* btnCancel = Component::CreateButton(panel, wxT("Cancel"), this, &ApplyActionsDialog::onCancel);
			sizerBar->Add(btnCancel, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE_VERTICAL|wxALL, 8);

			wxButton* btnPerform = Component::CreateButton(panel, wxT("Paste from the clipboard"), this, &ApplyActionsDialog::onPerform);
			sizerBar->Add(btnPerform, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE_VERTICAL|wxALL);

			sizerBar->Add(15, 5);
			btnPerform->SetDefault();
			btnPerform->SetFocus();
		}

		sizer->AddSpacer(10);
		sizer->Add(new wxStaticLine(this), 0, wxALL|wxEXPAND);
		sizer->Add(panel, 0, wxALL|wxEXPAND);

		pNotebook->add(panelScroll, wxT("system"), wxT("System Maps"));

		const Antares::Action::Context::Views& views = pContext->view;
		if (not views.empty() && false)
		{
			pNotebook->addSeparator();

			auto end = views.end();
			for (auto i = views.begin(); i != end; ++i)
			{
				auto& name = i->first;
				if (!name)
					continue;

				auto offset = name.find(':');
				if (offset >= name.size() - 1)
					continue;

				const wxString wxname = wxStringFromUTF8((const char *)name.c_str() + offset + 1);
				pNotebook->add(new Component::Panel(pNotebook), wxname, wxname);
			}
		}

		pNotebook->select(wxT("system"));

		sizer->Layout();
		SetSizer(sizer);
		sizer->Fit(this);

		wxSize p = GetSize();
		if (p.GetHeight() < 470)
			p.SetHeight(470);
		if (p.GetWidth() < 980)
			p.SetWidth(990);
		SetSize(p);

		Centre(wxBOTH);
	}



	void ApplyActionsDialog::onCancel(void*)
	{
		// ASync close
		Dispatcher::GUI::Close(this);
	}


	void ApplyActionsDialog::onPerform(void*)
	{
		// ASync close
		Dispatcher::GUI::Close(this);

		auto* form = new PerformerDialog(nullptr, pContext, pActions);
		Dispatcher::GUI::ShowModal(form);
	}





} // namespace Window
} // namespace Antares
