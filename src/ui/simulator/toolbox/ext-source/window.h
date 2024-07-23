/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__
#define __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__

#include <antares/study/study.h>
#include <yuni/thread/thread.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/dialog.h>
#include <action/action.h>

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
