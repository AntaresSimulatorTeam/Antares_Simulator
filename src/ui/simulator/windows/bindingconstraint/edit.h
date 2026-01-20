// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__
#define __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__

#include <antares/study/constraint.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/wx.h>

namespace Antares::Window
{
/*!
** \brief Dialog box for editing a binding constraint
*/
class BindingConstraintInfoEditor: public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Destructor
    **
    ** \param parent     The parent window
    ** \param constraint An existing constraint if any (edition mode). NULL otherwise (creation
    *mode)
    */
    BindingConstraintInfoEditor(wxWindow* parent, Data::BindingConstraint* constraint = NULL);

    //! Destructor
    virtual ~BindingConstraintInfoEditor()
    {
    }

    //@}

private:
    //! User: cancel the operation
    void onCancel(void*);
    //! User: save the modification / or / create a new constraint
    void onSave(void*);

private:
    //! The attached constraint if not null (edition mode), creation mode otherwise
    Data::BindingConstraint* pConstraint;
    //! Control: The new caption of the constraint
    wxTextCtrl* pCaption;
    //! Control: The new comments of the constraint
    wxTextCtrl* pComments;
    //! Control: Enabled / disabled
    wxCheckBox* pEnabled;
    //! Control: Type of the constraint (hourly, daily..., only on creation mode)
    wxChoice* pType;
    //! Control: Operator of the constraint (>,<, >=...)
    wxChoice* pOperator;

}; // class BindingConstraintInfoEditor

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__
