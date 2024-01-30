/*
** Copyright 2007-2023 RTE
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
#include "antares/study/study.h"
#include "../../../config.h"
#include "../../toolbox/resources.h"
#include "../../windows/aboutbox.h"
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
        cmd << wxT("xdg-open \"") << Resources::WxFindFile(url) << wxT("\"");

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

void ApplWnd::evtOnHelpPDFSystemMapEditorReferenceGuide(wxCommandEvent&)
{
    OpenPDF("help/system-map-editor-reference-guide.pdf");
}

void ApplWnd::evtOnHelpPDFExamplesLibrary(wxCommandEvent&)
{
    OpenPDF("help/antares-examples-library.pdf");
}

void ApplWnd::evtOnHelpOnlineDocumentation(wxCommandEvent&)
{
    wxLaunchDefaultBrowser(ANTARES_ONLINE_DOC);
}

} // namespace Forms
} // namespace Antares
