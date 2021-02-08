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
#ifndef __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__
#define __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__

#include <antares/wx-wrapper.h>
#include <antares/study.h>
#include <yuni/thread/thread.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/dialog.h>
#include <antares/study/action/action.h>

namespace Antares
{
namespace Window
{
class ApplyActionsDialog : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    ApplyActionsDialog(wxWindow* parent,
                       const Antares::Action::Context::Ptr& context,
                       const Antares::Action::IAction::Ptr& root);

    //! Destructor
    virtual ~ApplyActionsDialog()
    {
    }
    //@}

private:
    //! Event: The user asked to cancel the operation
    void onCancel(void*);
    //! Event: Performing the operation
    void onPerform(void*);

private:
    //! The target study
    Antares::Action::Context::Ptr pContext;
    //! The tree of the actions to perform
    Antares::Action::IAction::Ptr pActions;

    wxStaticText* pLblInfos;

}; // class ApplyActionsDialog

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__
