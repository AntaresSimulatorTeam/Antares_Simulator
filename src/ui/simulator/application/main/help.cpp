/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "../main.h"
#include "../menus.h"
#include "antares/study/study.h"
#include <antares/config/config.h>
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
