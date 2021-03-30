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

#include "../../../internet/license.h"
#include "../../windows/proxy/proxysetup.h"
#include "../../../internet/limits.h"
#include "../../windows/message.h"

#define ANTARES_CHECKING_LICENSE_TEXT "Checking Internet Access..."

// forward
static void CheckAntaresLicense(bool startupwizard);
static void AskForTryingAgain(bool startupwizard);

static void DisconnectionFromLicenseServer()
{
    Bind<void()> callback;
    callback.bind(&AskForTryingAgain, false);
    Antares::Dispatcher::GUI::Post(callback);
}

static void RenableTheMainForm()
{
    auto* mainfrm = Antares::Forms::ApplWnd::Instance();
    if (mainfrm)
    {
        mainfrm->Enable(true);
        mainfrm->resetDefaultStatusBarText();
    }
}

static void DisableTheMainForm()
{
    auto* mainfrm = Antares::Forms::ApplWnd::Instance();
    if (mainfrm)
    {
        mainfrm->Enable(false);
        mainfrm->resetDefaultStatusBarText();
    }
}

static void LicenseOnLineIsNotValid(bool startupwizard)
{
    assert(wxIsMainThread() == true && "Must be ran from the main thread");

    auto* mainfrm = Antares::Forms::ApplWnd::Instance();
    if (mainfrm)
    {
        // restoring customer caption
        // reset status bar
        Antares::License::Limits::customerCaption.clear();
        mainfrm->resetDefaultStatusBarText();

        Antares::Window::LicenseCouldNotConnectToInternetServer form(mainfrm);
        form.ShowModal();

        if (not form.canceled())
        {
            // Ok, let's try again

            // The status bar will be reset by the dispatcher
            Antares::License::Limits::customerCaption = ANTARES_CHECKING_LICENSE_TEXT;
            mainfrm->resetDefaultStatusBarText();

            // check the license informations
            // (in another thread - it can take some time)
            Bind<void()> callback;
            callback.bind(&CheckAntaresLicense, startupwizard);
            Antares::Dispatcher::Post(callback);
        }
        else
        {
            // The status bar will be reset by the dispatcher
            Antares::License::Limits::customerCaption = "Aborting connexion...";
            mainfrm->resetDefaultStatusBarText();

            // check the license informations
            // (in another thread - it can take some time)
            Antares::License::statusOnline = Antares::License::Status::stNotRequested;
            Bind<void()> callback;
            callback.bind(&CheckAntaresLicense, startupwizard);
            Antares::Dispatcher::Post(callback);
        }
    }
    else
    {
        // what ?
        assert(false and "we must have the main window here !");
        exit(1); // just in case
    }
}

static void AskForTryingAgain(bool startupwizard)
{
    auto* mainfrm = Antares::Forms::ApplWnd::Instance();
    if (mainfrm)
    {
        const wxChar* msg = wxT("Error");

        switch (Antares::License::lastError)
        {
        case Antares::License::errLSHostDown:
        {
            msg = wxT("The internet server seems down. Please retry in a few minutes.\r\nPlease "
                      "contact your system administrator if the problem persists.");
            break;
        }
        case Antares::License::errLSTooManyLicense:
        {
            msg = wxT("Exceeded maximum licenses.\r\nAll tokens provided by the internet Server "
                      "are currently used");
            break;
        }
        default:
        {
            msg = wxT("Error");
            break;
        }
        }

        Antares::Window::Message message(mainfrm,
                                         wxT("Floating license"),
                                         wxT("Error with the internet Server"),
                                         msg,
                                         "images/misc/error.png");

        message.add(Antares::Window::Message::btnRetry, true);
        message.add(Antares::Window::Message::btnQuit);

        switch (message.showModal())
        {
        case Antares::Window::Message::btnRetry:
        {
            // Ok, let's try again

            // The status bar will be reset by the dispatcher
            Antares::License::Limits::customerCaption = ANTARES_CHECKING_LICENSE_TEXT;
            mainfrm->resetDefaultStatusBarText();
            mainfrm->Enable(false);

            // check the license informations
            // (in another thread - it can take some time)
            Bind<void()> callback;
            callback.bind(&CheckAntaresLicense, startupwizard);
            Antares::Dispatcher::Post(callback);
            break;
        }
        default:
        {
            // The status bar will be reset by the dispatcher
            Antares::License::Limits::customerCaption = "Aborting...";
            mainfrm->resetDefaultStatusBarText();

            // the operation has been canceled by the user
            Antares::Dispatcher::GUI::Close(mainfrm);
            break;
        }
        }
    }
    else
    {
        // what ?
        assert(false and "we must have the main window here !");
        exit(1); // just in case
    }
}

static void CheckAntaresLicense(bool startupwizard)
{
    // re-enable the mainform
    Bind<void()> callback;
    callback.bind(&RenableTheMainForm);
    Antares::Dispatcher::GUI::Post(callback);

    // Ok the license is valid, let's continue !
    if (startupwizard)
    {
        auto* mainfrm = Antares::Forms::ApplWnd::Instance();
        Bind<void()> callback;
        callback.bind(mainfrm, &Antares::Forms::ApplWnd::startAntares);
        Antares::Dispatcher::GUI::Post(callback); // ms, arbitrary
    }
}

static void DispatchCheckAntaresLicense()
{
    assert(wxIsMainThread() == true and "Must be ran from the main thread");

    // rebind events
    Antares::License::onDisconnect.clear();
    Antares::License::onDisconnect.connect(&DisconnectionFromLicenseServer);

    // The status bar will be reset by the dispatcher
    Antares::License::Limits::customerCaption = ANTARES_CHECKING_LICENSE_TEXT;

    // Not need to check the permission for a starting from simulator
    Antares::License::hasSimulatorAuthorization = true;

    // check the license informations
    // (in another thread - it can take some time)
    Bind<void()> callback;
    callback.bind(&CheckAntaresLicense, true);
    Antares::Dispatcher::Post(callback);
}
