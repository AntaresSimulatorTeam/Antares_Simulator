// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__
#define __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__

#include <antares/study/study.h>
#include "../application/study.h"

#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/collpane.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/collpane.h>
#include <wx/valtext.h>

namespace Antares::Window
{
/*!
** \brief `Save As` Dialog
*/
class SaveAs final: public wxDialog
{
public:
    /*!
    ** \brief Execute the `SaveAs` dialog and save the given study accordingly
    **
    ** \param parent The parent window
    ** \param study The study
    ** \return True if the study has been saved, false otherwise (cancelled for example)
    */
    static SaveResult Execute(wxFrame* parent, Data::Study::Ptr study);

public:
    enum
    {
        mnIDCollapsePanel = wxID_HIGHEST + 1,
        mnIDEditStudyName,
        mnIDEditStudyFolderName,
        mnIDBrowse,
    };

public:
    // \name Constructors & Destructor
    //@{
    /*!
    ** \brief Constructor with a parent window and a study
    */
    explicit SaveAs(wxFrame* parent);
    /*!
    ** \brief Constructor with a parent window and a study
    */
    SaveAs(wxFrame* parent, Data::Study::Ptr study);
    //! Destructor
    virtual ~SaveAs();
    //@}

    //! \name Study
    //@{
    //! Get the attached study
    Data::Study::Ptr study();
    //! Get the attached study
    const Data::Study::Ptr study() const;
    //@}

    /*!
    ** \brief Get if the study has been saved
    */
    SaveResult result() const;

private:
    //! Create components
    void internalCreateComponents();

    void onCollapsePanel(wxCollapsiblePaneEvent& evt);
    void onStudyNameChanged(wxCommandEvent& evt);
    void onStudyFolderNameChanged(wxCommandEvent& evt);
    void onBrowse(wxCommandEvent& evt);

    //! Event: The user has cancelled the operation
    void onCancel(void*);
    //! Event: Save the study !
    void onSave(void*);

    void propCaption(const wxString& text, bool bold = false);
    wxTextCtrl* propEdit(const wxString& defaultValue = wxString(), int id = wxID_ANY);
    wxWindow* propEdit(wxWindow* control);
    wxTextCtrl* propEdit(const wxString& defaultValue,
                         const wxTextValidator& validator,
                         int id = wxID_ANY);

private:
    //! The study
    Data::Study::Ptr pStudy;
    //! Grid Sizer
    wxFlexGridSizer* pGridSizer;
    //!
    wxWindow* pParentProperties;
    //! Control for the name of the study
    wxTextCtrl* pStudyName;
    //! Control for the name of the study folder
    wxTextCtrl* pStudyFolderName;
    //! Control for the path
    wxTextCtrl* pFolder;
    //! True if the user set a custom study folder name
    bool pCustomFolderName;

    //! Checkbox for copying or not the output folder
    wxCheckBox* pCopyOutput;
    //! Checkbox for copying or not the user data folder
    wxCheckBox* pCopyUserData;
    //! Checkbox for copying or not the user data folder
    wxCheckBox* pCopyLogs;
    //! Result
    SaveResult pResult;
    // Event Table
    DECLARE_EVENT_TABLE()

}; // class SaveAs

} // namespace Antares::Window

#include "saveas.hxx"

#endif // __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__
