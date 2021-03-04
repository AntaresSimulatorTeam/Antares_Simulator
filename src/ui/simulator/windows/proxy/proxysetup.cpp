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

#include "proxysetup.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>

#include "../../toolbox/create.h"
#include "../../toolbox/resources.h"
#include "../../application/menus.h"
#include "../message.h"
#include "../../../../internet/license.h"

#include <ui/common/component/panel.h>
#include <antares/logs.h>
#include "../startupwizard.h"

using namespace Yuni;

#ifdef YUNI_OS_WINDOWS
#define FIXED_FONTNAME wxT("Consolas")
#define FIXED_FONTNAME_FALLBACK wxT("Lucida Console")
#define FIXED_FONT_SIZE 0
#else
#define FIXED_FONTNAME wxT("Monospace")
#define FIXED_FONTNAME_FALLBACK wxT("Monospace")
#define FIXED_FONT_SIZE -1
#endif

namespace Antares
{
namespace Window
{
LicenseCouldNotConnectToInternetServer::LicenseCouldNotConnectToInternetServer(wxWindow* parent) :
 wxDialog(parent,
          wxID_ANY,
          wxT(""),
          wxDefaultPosition,
          wxDefaultSize,
          wxCLOSE_BOX | wxCAPTION | wxCLIP_CHILDREN),
 pEditProxyHost(nullptr),
 pEditProxyPort(nullptr),
 pEditProxyLogin(nullptr),
 pEditProxyPass(nullptr),
 pCanceled(true)
{
    // TODO : a lot of pointer variables are not destroyed after usage here : sizers, titles, ...
    // TODO : They have to be deleted to avoid memory leaks.

    assert(parent);

    // Background color
    wxColour defaultBgColor = GetBackgroundColour();
    wxColour bgColor(255, 255, 255);
    SetBackgroundColour(bgColor);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->AddSpacer(30);

    auto* hz = new wxBoxSizer(wxHORIZONTAL);
    hz->AddSpacer(30);
    hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/server.png"),
            0,
            wxALL | wxALIGN_TOP);
    sizer->Add(hz, 1, wxALL | wxEXPAND);

    auto* contentSizer = new wxBoxSizer(wxVERTICAL);
    hz->Add(contentSizer, 1, wxALL | wxEXPAND);
    hz->AddSpacer(30);

    auto* titlespacer = new wxBoxSizer(wxHORIZONTAL);
    auto* title = Component::CreateLabel(
      this,
      wxT("Impossible to connect to the internet with current settings.\nPlease check or update "
          "proxy parameters to use for this location"),
      false,
      false,
      +1);
    auto* subtitle = Component::CreateLabel(
      this,
      wxT("No internet access or connection controlled by local proxy.\nPlease fill out your proxy "
          "parameters to allow connection."),
      false,
      false);
    subtitle->Enable(false);
    pOffline_title
      = Component::CreateLabel(this,
                               wxT("If you wish to stay offline in all future sessions,\n"
                                   "click on \"Cancel\", and select \"Continue offline\"\n"
                                   "in the Antares help (\"?\") menu."),
                               false,
                               false);
    contentSizer->AddSpacer(20);
    contentSizer->Add(titlespacer, 0, wxALL | wxEXPAND);
    contentSizer->AddSpacer(40);

    auto* subtitlespacer = new wxBoxSizer(wxVERTICAL);
    subtitlespacer->AddStretchSpacer();
    subtitlespacer->Add(title, 0, wxLEFT);
    subtitlespacer->AddSpacer(3);
    subtitlespacer->Add(subtitle, 0, wxLEFT);
    subtitlespacer->AddSpacer(10);
    subtitlespacer->Add(pOffline_title, 0, wxLEFT);
    subtitlespacer->AddStretchSpacer();
    titlespacer->AddSpacer(10);
    titlespacer->Add(subtitlespacer, 1, wxALL | wxALIGN_CENTER_VERTICAL | wxLEFT);

    auto* proxySizerDiv = new wxBoxSizer(wxHORIZONTAL);
    proxySizerDiv->AddSpacer(10);
    auto* proxySizer = new wxBoxSizer(wxVERTICAL);
    proxySizerDiv->Add(proxySizer, 1, wxALL | wxEXPAND);
    contentSizer->Add(proxySizerDiv, 1, wxALL | wxEXPAND);

    pProxyEnabled = new wxCheckBox(this, wxID_ANY, wxT(" Activate proxy"));
    proxySizer->Add(pProxyEnabled, 0, wxALL | wxEXPAND);
    proxySizer->AddSpacer(15);

    pFlexSizer = new wxFlexGridSizer(2, 2, 5);
    auto* loginSizer = new wxBoxSizer(wxHORIZONTAL);
    // loginSizer->AddSpacer(25);
    loginSizer->Add(pFlexSizer, 1, wxALL | wxEXPAND);
    proxySizer->Add(loginSizer, 1, wxALL | wxEXPAND);

    pEditProxyHost = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(170, -1));
    pEditProxyPort = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(60, -1));
    pEditProxyLogin = new wxTextCtrl(this, wxID_ANY, wxT(""));
    pEditProxyPass
      = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(170, -1), wxTE_PASSWORD);

    pLblProxyHost = Component::CreateLabel(this, wxT("Proxy IP address:"));
    pLblProxyPort = Component::CreateLabel(this, wxT("Port:"));
    pLblProxyLogin = Component::CreateLabel(this, wxT("Username:"));
    pLblProxyPass = Component::CreateLabel(this, wxT("Password:"));

    // display proxy parameters
    auto& proxy = Antares::License::proxy;
    // read proxy parameters from file
    proxy.loadProxyFile();
    pProxyEnabled->SetValue(proxy.enabled);
    pEditProxyHost->SetValue(wxStringFromUTF8(proxy.host));
    pEditProxyPort->SetValue(wxStringFromUTF8(proxy.port));
    pEditProxyLogin->SetValue(wxStringFromUTF8(proxy.login));
    pEditProxyPass->SetValue(wxStringFromUTF8(proxy.password));

    auto* urlDiv = new wxBoxSizer(wxHORIZONTAL);
    urlDiv->Add(pEditProxyHost, 1, wxALL | wxEXPAND);
    urlDiv->AddSpacer(10);
    urlDiv->Add(pLblProxyPort, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxLEFT);
    urlDiv->AddSpacer(5);
    urlDiv->Add(pEditProxyPort, 0, wxALL | wxEXPAND);

    pFlexSizer->Add(pLblProxyHost, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxLEFT);
    pFlexSizer->Add(urlDiv, 1, wxALL | wxEXPAND);
    pFlexSizer->Add(pLblProxyLogin, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxLEFT);
    pFlexSizer->Add(pEditProxyLogin, 1, wxALL | wxEXPAND);
    pFlexSizer->Add(pLblProxyPass, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxLEFT);
    pFlexSizer->Add(pEditProxyPass, 1, wxALL | wxEXPAND);

    contentSizer->AddSpacer(40);

    // Buttons
    auto* panel = new Component::Panel(this);
    panel->SetBackgroundColour(defaultBgColor);
    wxBoxSizer* pnlSizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pnlSizerBtnsV = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(pnlSizerBtnsV);
    pnlSizerBtnsV->AddSpacer(8);
    pnlSizerBtnsV->Add(pnlSizerBtns, 1, wxALL | wxEXPAND);
    pnlSizerBtnsV->AddSpacer(8);

    pnlSizerBtns->AddSpacer(25);

    pnlSizerBtns->AddStretchSpacer();
    auto* pBtnValidate = Antares::Component::CreateButton(
      panel, wxT(" Connect "), this, &LicenseCouldNotConnectToInternetServer::onProceed);
    pBtnValidate->SetDefault();

    auto* pBtnCancel = Antares::Component::CreateButton(
      panel, wxT(" Cancel "), this, &LicenseCouldNotConnectToInternetServer::onClose);

    pnlSizerBtns->Add(pBtnCancel, 0, wxALL | wxEXPAND);
    pnlSizerBtns->AddSpacer(5);
    pnlSizerBtns->Add(pBtnValidate, 0, wxALL | wxEXPAND);
    pnlSizerBtns->Add(25, 5);

    sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL | wxEXPAND);
    sizer->Add(panel, 0, wxALL | wxEXPAND);

    // refresh
    Connect(GetId(),
            wxEVT_MOTION,
            wxMouseEventHandler(LicenseCouldNotConnectToInternetServer::onInternalMotion),
            NULL,
            this);

    pProxyEnabled->Connect(
      pProxyEnabled->GetId(),
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(LicenseCouldNotConnectToInternetServer::evtToggleUseProxy),
      nullptr,
      this);

    SetSizer(sizer);
    sizer->Layout();

    // Recentre the window
    sizer->Fit(this);
    Centre(wxBOTH);

    toggleProxySettings();
}

LicenseCouldNotConnectToInternetServer::~LicenseCouldNotConnectToInternetServer()
{
    Component::Spotlight::FrameClose();
    delete pOffline_title;
}

void LicenseCouldNotConnectToInternetServer::onClose(void*)
{
    Dispatcher::GUI::Close(this);
}

void LicenseCouldNotConnectToInternetServer::onInternalMotion(wxMouseEvent&)
{
    Component::Panel::OnMouseMoveFromExternalComponent();
}

void LicenseCouldNotConnectToInternetServer::onProceed(void*)
{
    auto& proxy = Antares::License::proxy;
    // update proxy value
    proxy.enabled = pProxyEnabled->GetValue();
    if (proxy.enabled)
    {
        wxStringToString(pEditProxyHost->GetValue(), proxy.host);
        wxStringToString(pEditProxyLogin->GetValue(), proxy.login);
        wxStringToString(pEditProxyPass->GetValue(), proxy.password);
        wxStringToString(pEditProxyPort->GetValue(), proxy.port);
    }

    // busy !
    ::wxBeginBusyCursor();

    // check if the proxy is correct
    if (!proxy.check() && proxy.enabled)
    {
        logs.error() << "Incorrect or missing proxy settings, please check your proxy settings";
        ::wxEndBusyCursor();
        return;
    }

    // save parameters in the file
    proxy.saveProxyFile();
    // if check license on the server failed
    if (not Antares::License::CheckOnlineLicenseValidity(Data::versionLatest, true))
    {
        String errMessage;
        String errType;
        Antares::License::WriteLastError(errMessage, errType);

        wxString wxMessage = wxString(errMessage.c_str(), wxConvUTF8);
        wxString wxType = wxString(errType.c_str(), wxConvUTF8);

        // dispaly error message
        Antares::Window::Message message(this, wxT(""), wxType, wxMessage, "images/misc/error.png");
        message.add(Window::Message::btnContinue);
        message.showModal();

        ::wxEndBusyCursor();
        return;
    }

    ::wxEndBusyCursor();

    pCanceled = false;
    // finish - let's continue !
    Dispatcher::GUI::Close(this);
    // display the next window (welcome page or proxy setup page)
    StartupWizard::Show();
}

void LicenseCouldNotConnectToInternetServer::toggleProxySettings()
{
    // show poxy parameters
    bool hasProxy = pProxyEnabled->GetValue();

    pEditProxyHost->Show(hasProxy);
    pEditProxyPort->Show(hasProxy);
    pEditProxyLogin->Show(hasProxy);
    pEditProxyPass->Show(hasProxy);

    pLblProxyHost->Show(hasProxy);
    pLblProxyPort->Show(hasProxy);
    pLblProxyLogin->Show(hasProxy);
    pLblProxyPass->Show(hasProxy);

    auto* sizer = pLblProxyHost->GetSizer();
    if (sizer)
        sizer->Layout();
    sizer = pLblProxyLogin->GetSizer();
    if (sizer)
        sizer->Layout();

    pFlexSizer->Layout();

    auto* mainsizer = GetSizer();
    if (mainsizer)
        mainsizer->Layout();
}

void LicenseCouldNotConnectToInternetServer::evtToggleUseProxy(wxCommandEvent&)
{
    toggleProxySettings();
}

bool LicenseCouldNotConnectToInternetServer::canceled() const
{
    return pCanceled;
}

} // namespace Window
} // namespace Antares
