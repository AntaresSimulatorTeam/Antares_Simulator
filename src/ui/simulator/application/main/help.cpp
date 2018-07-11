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
#include <yuni/core/string.h>
#include "../main.h"
#include "../menus.h"
#include "../study.h"
#include "../../../config.h"
#include "../../toolbox/resources.h"
#include "../../windows/aboutbox.h"
#include "../../windows/proxy/proxysetup.h"
#include "../../../internet/license.h"
#include "../../toolbox/dispatcher/study.h"
#include "../../windows/message.h"
#include <antares/study/version.h>
#include <wx/utils.h>

using namespace Yuni;



namespace Antares
{
namespace Forms
{

	static inline void OpenPDF(const AnyString& url)
	{
		logs.info() << "opening PDF " << url;
		wxString cmd;

		if (System::windows)
		{
			String u = url;
			u.replace('/', '\\');
			cmd << wxT("explorer.exe \"") << Resources::WxFindFile(u) << wxT("\"");
		}
		else
			cmd << wxT("gnome-open \"") << Resources::WxFindFile(url) << wxT("\"");

		wxExecute(cmd);
	}



	/*void ApplWnd::evtOnHelpVisitRTEWebsite(wxCommandEvent&)
	{
		logs.info() << "opening url " << ANTARES_WEBSITE;
		wxString url(wxT(ANTARES_WEBSITE));
		if (not wxLaunchDefaultBrowser(url, wxBROWSER_NEW_WINDOW))
		{
			String u;
			wxStringToString(url, u);
			logs.error() << "Failed to open the url `" << u << '`';
		}
	}*/


	void ApplWnd::evtOnHelpAbout(wxCommandEvent&)
	{
		Forms::Disabler<ApplWnd> disabler(*this);
		{
			Window::AboutBox about(this);
			about.ShowModal();
		}
	}

	void ApplWnd::evtOnHelpPDFGeneralReferenceGuide(wxCommandEvent&)
	{
		OpenPDF("help/antares-general-reference-guide.pdf");
	}


	void ApplWnd::evtOnHelpPDFSystemMapEditorReferenceGuide(wxCommandEvent&)
	{
		OpenPDF("help/system-map-editor-reference-guide.pdf");
	}



	void ApplWnd::evtOnHelpContinueOnline(wxCommandEvent&)
	{
		setGDPRStatus(true);
	}
	
	void ApplWnd::evtOnHelpContinueOffline(wxCommandEvent&)
	{
		setGDPRStatus(false);
	}

	void ApplWnd::evtOnShowID(wxCommandEvent&)
	{
		Antares::License::Properties hostproperties;
		Antares::License::Properties licenseproperties;
		Yuni::String tmp;

		Antares::License::RetrieveHostProperties(hostproperties, tmp);
		auto hostid = hostproperties[(tmp = "k")];

		Window::Message message(this, wxT(""),
			hostid.c_str(), "", "images/128x128/antares.png");
		message.add(Window::Message::btnOk, true);
		message.showModal();
	}

	
} // namespace Forms
} // namespace Antares
