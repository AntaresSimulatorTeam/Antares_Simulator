// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


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

namespace Antares::Forms
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
    {
        cmd << wxT("xdg-open \"") << Resources::WxFindFile(url) << wxT("\"");
    }

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

} // namespace Antares::Forms
