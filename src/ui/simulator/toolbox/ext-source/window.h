// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__
#define __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__

#include <antares/study/study.h>
#include <yuni/thread/thread.h>
#include <wx/stattext.h>
#include <wx/timer.h>
#include <wx/dialog.h>
#include <action/action.h>

namespace Antares::Window
{
class ApplyActionsDialog: public wxDialog
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

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_EXT_SOURCE_WINDOW_H__
