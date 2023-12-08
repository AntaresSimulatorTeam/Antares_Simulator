/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__

#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

namespace Antares
{
namespace Window
{
namespace Options
{
/*!
** \brief Startup Wizard User Interface
*/
class ConfigureTempFolder final : public wxDialog
{
public:
    enum IDs
    {
        mnIDDefault = wxID_HIGHEST + 1,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    ConfigureTempFolder(wxWindow* parent);
    //! Destructor
    virtual ~ConfigureTempFolder();
    //@}

private:
    void reset(bool useDefault);
    void onSave(void*);
    void onCancel(void*);
    void evtDefault(wxCommandEvent& evt);
    void onBrowse(void*);

private:
    wxButton* pBtnCancel;
    wxButton* pBtnGo;
    wxButton* pBtnBrowse;
    wxTextCtrl* pPath;
    wxCheckBox* pDefaults;
    wxStaticText* pText;

    // Table
    DECLARE_EVENT_TABLE()

}; // class ConfigureTempFolder

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__
