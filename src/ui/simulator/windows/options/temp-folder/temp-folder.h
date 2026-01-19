// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__

#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>

namespace Antares::Window::Options
{
/*!
** \brief Startup Wizard User Interface
*/
class ConfigureTempFolder final: public wxDialog
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

} // namespace Antares::Window::Options

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_TEMP_FOLDER_H__
