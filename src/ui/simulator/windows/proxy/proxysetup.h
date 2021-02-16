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
#ifndef __ANTARES_APPLICATION_WINDOWS_PROXY_SETUP_H__
#define __ANTARES_APPLICATION_WINDOWS_PROXY_SETUP_H__

#include <antares/wx-wrapper.h>
#include "../../toolbox/components/button.h"
#include <wx/dialog.h>
#include <wx/checkbox.h>

namespace Antares
{
namespace Window
{
/*!
** \brief Startup Wizard User Interface
*/
class LicenseCouldNotConnectToInternetServer final : public wxDialog
{
public:
    /*!
    ** \brief Show an instance of proxy setup page
    */
    // static void ShowProxySetup();

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    LicenseCouldNotConnectToInternetServer(wxWindow* parent);
    //! Destructor
    virtual ~LicenseCouldNotConnectToInternetServer();
    //@}

    //! Get if the operation has been canceled by the user
    bool canceled() const;

private:
    void onClose(void*);
    void onProceed(void*);
    void toggleProxySettings();
    void evtToggleUseProxy(wxCommandEvent&);
    void onInternalMotion(wxMouseEvent&);

private:
    wxCheckBox* pProxyEnabled;
    wxCheckBox* pRemoveDialogBox;
    wxTextCtrl* pEditProxyHost;
    wxTextCtrl* pEditProxyPort;
    wxTextCtrl* pEditProxyLogin;
    wxTextCtrl* pEditProxyPass;
    wxWindow* pLblProxyHost;
    wxWindow* pLblProxyPort;
    wxWindow* pLblProxyLogin;
    wxWindow* pLblProxyPass;
    wxSizer* pFlexSizer;
    bool pCanceled;

}; // class LicenseCoudtNotConnectToInternetServer

} // namespace Window
} // namespace Antares

#endif //__ANTARES_APPLICATION_WINDOWS_PROXY_SETUP_H__
