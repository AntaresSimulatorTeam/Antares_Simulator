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

#include "temp-folder.h"
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/config.h>
#include <antares/memory/memory.h>
#include <antares/logs.h>
#include "../../../toolbox/components/wizardheader.h"
#include "../../../toolbox/create.h"
#include <yuni/io/directory.h>
#include <antares/sys/policy.h>

using namespace Yuni;



namespace Antares
{
namespace Window
{
namespace Options
{

	BEGIN_EVENT_TABLE(ConfigureTempFolder, wxDialog)
		EVT_CHECKBOX(mnIDDefault, ConfigureTempFolder::evtDefault)
	END_EVENT_TABLE()





	ConfigureTempFolder::ConfigureTempFolder(wxWindow* parent) :
		wxDialog(parent, wxID_ANY, wxT("Options"), wxDefaultPosition, wxDefaultSize,
			wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN),
		pBtnCancel(nullptr),
		pBtnGo(nullptr),
		pBtnBrowse(nullptr),
		pPath(nullptr),
		pDefaults(nullptr),
		pText(nullptr)
	{
		assert(NULL != parent);
		auto* config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));

		// Title of the Form
		SetLabel(wxT("Options"));
		SetTitle(wxT("Options"));


		auto* sizer = new wxBoxSizer(wxVERTICAL);

		// Header
		sizer->Add(Toolbox::Components::WizardHeader::Create(this, wxT("Options"),
			"images/32x32/settings.png", wxT("Configure the temporary folder")), 0, wxALL|wxEXPAND | wxFIXED_MINSIZE);


		// Space
		sizer->AddSpacer(5);


		auto* s = new wxFlexGridSizer(1, 1, 0);
		s->SetFlexibleDirection(wxHORIZONTAL);
		s->AddGrowableCol(0, 1);

		bool readonly = (! LocalPolicy::ReadAsBool("allow_custom_swap_folder", true));

		if (readonly)
		{
			wxStaticText* info = Component::CreateLabel(this,
				wxT("The local policy of this system does not permit you to modify these settings."),
				true);
			s->Add(info, 0, wxALL|wxEXPAND | wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			s->AddSpacer(15);
		}

		if (not readonly)
		{
			pDefaults = new wxCheckBox(this, mnIDDefault, wxT(" Use the default settings (System Temporary Directory) *"));
			bool b;
			if (config->Read(wxT("tmp/usedefaults"), &b, true))
				pDefaults->SetValue(b);
			else
				pDefaults->SetValue(true);

			s->Add(pDefaults, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}

		if (not readonly)
		{
			pText = new wxStaticText(this, wxID_ANY, wxT("Temporary folder : "));
			s->Add(pText, 0, wxRIGHT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

			wxString pth;
			bool b;
			if (config->Read(wxT("tmp/usedefaults"), &b, true))
				pth = config->Read(wxT("tmp/path"), wxEmptyString);
			if (pth.empty())
				pth = wxStringFromUTF8(Antares::memory.cacheFolder());

			pPath = new wxTextCtrl(this, wxID_ANY, pth, wxDefaultPosition);
			pBtnBrowse = Antares::Component::CreateButton(this, wxT("Browse"), this, &ConfigureTempFolder::onBrowse);
			wxSizer* sz = new wxBoxSizer(wxHORIZONTAL);
			sz->Add(pBtnBrowse, 0, wxEXPAND|wxALL);
			sz->AddSpacer(4);
			sz->Add(pPath, 1, wxEXPAND | wxALL);
			s->Add(sz, 1, wxEXPAND | wxALL);
		}

		sizer->Add(s, 0, wxALL|wxEXPAND, 20);
		sizer->AddSpacer(60);

		if (not readonly)
		{
			const size_t diskSpaceUsed = Antares::memory.memoryCapacity() / 1024 / 1024;
			wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
			hs->Add(20, 5);
			wxStaticText* info = Component::CreateLabel(this,
				wxString(wxT("Disk space currently used by swap files : ")) << diskSpaceUsed << wxT("Mo"),
				false, true);
			hs->Add(info, 1, wxALL|wxEXPAND);
			hs->Add(20, 5);
			sizer->Add(hs, 0, wxALL|wxEXPAND);
		}
		if (not readonly)
		{
			wxSizer* hs = new wxBoxSizer(wxHORIZONTAL);
			hs->Add(20, 5);
			wxStaticText* info = Component::CreateLabel(this,
				wxT("* Antares must be restarted for the changes to take effect"),
				false, true);
			hs->Add(info, 1, wxALL|wxEXPAND);
			hs->Add(20, 5);
			sizer->Add(hs, 0, wxALL|wxEXPAND);
		}


		// Buttons
		auto* pnlBtns = new wxBoxSizer(wxHORIZONTAL);
		pnlBtns->AddStretchSpacer();
		pBtnCancel = Antares::Component::CreateButton(this, (readonly) ? wxT("  Close  ") : wxT("Cancel"), this, &ConfigureTempFolder::onCancel);
		pBtnCancel->SetDefault();
		pnlBtns->Add(pBtnCancel);

		if (not readonly)
		{
			pnlBtns->AddSpacer(5);
			pBtnGo = Antares::Component::CreateButton(this, wxT(" Save and Close "), this, &ConfigureTempFolder::onSave);
			pnlBtns->Add(pBtnGo, 0, wxALL|wxEXPAND);
		}
		pnlBtns->Add(25, 5);

		if (not readonly)
			sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL|wxEXPAND, 8);
		sizer->Add(pnlBtns, 0, wxALL|wxEXPAND);
		sizer->AddSpacer(8);

		if (pDefaults)
			reset(pDefaults->GetValue());

		SetSizer(sizer);
		sizer->Layout();

		// Recentre the window
		sizer->Fit(this);
		if (not readonly)
		{
			wxSize p = GetSize();
			const int wantedSize = 500;
			if (p.GetWidth() != wantedSize)
				p.SetWidth(wantedSize);
			SetSize(p);
		}
		Centre(wxBOTH);

		delete config;
	}


	ConfigureTempFolder::~ConfigureTempFolder()
	{
	}


	void ConfigureTempFolder::onSave(void*)
	{
		if (!pDefaults or !pPath)
			return;

		auto* config = new wxConfig(wxT(LOG_APPLICATION_NAME), wxT(LOG_APPLICATION_VENDOR));
		if (not pDefaults->GetValue())
		{
			String path;
			wxStringToString(pPath->GetValue(), path);
			if (IO::IsRelative(path))
			{
				logs.error() << "The directory must be absolute";
				delete config;
				return;
			}
			if (IO::Directory::Exists(path))
			{
				config->Write(wxT("tmp/path"), pPath->GetValue());
				config->Write(wxT("tmp/usedefaults"), false);
				Antares::memory.cacheFolder(path);
				Dispatcher::GUI::Close(this);
			}
			else
				logs.error() << "The directory must exist";
		}
		else
		{
			config->Write(wxT("tmp/usedefaults"), true);
			Dispatcher::GUI::Close(this);
		}
		delete config;
	}


	void ConfigureTempFolder::onCancel(void*)
	{
		Dispatcher::GUI::Close(this);
	}


	void ConfigureTempFolder::evtDefault(wxCommandEvent& evt)
	{
		reset(evt.GetInt() != 0);
	}


	void ConfigureTempFolder::onBrowse(void*)
	{
		if (pPath)
		{
			const wxString& path = pPath->GetValue();
			wxDirDialog* dlg = new wxDirDialog(this, wxT("Temporary folder"), path, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
			if (wxID_OK == dlg->ShowModal())
				pPath->SetValue(dlg->GetPath());
			delete dlg;
		}
	}


	void ConfigureTempFolder::reset(bool useDefault)
	{
		if (pBtnBrowse and pPath and pText)
		{
			pBtnBrowse->Enable(!useDefault);
			pPath->Enable(!useDefault);
			pText->Enable(!useDefault);
		}
	}





} // namespace Options
} // namespace Window
} // namespace Antares
