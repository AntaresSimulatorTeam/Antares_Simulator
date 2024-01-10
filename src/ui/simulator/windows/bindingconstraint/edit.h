/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
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
*/
#ifndef __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__
#define __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__

#include <antares/study/constraint.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>
#include <wx/wx.h>

namespace Antares
{
namespace Window
{
/*!
** \brief Dialog box for editing a binding constraint
*/
class BindingConstraintInfoEditor : public wxDialog
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

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_BINDING_CONSTRAINT_EDIT_H__
