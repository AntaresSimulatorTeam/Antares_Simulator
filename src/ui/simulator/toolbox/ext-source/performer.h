// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__
#define __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__

#include <antares/study/study.h>
#include <yuni/thread/thread.h>
#include <wx/timer.h>
#include <wx/dialog.h>
#include <action/action.h>
#include "../components/progressbar.h"
#include <atomic>
#include <mutex>

namespace Antares::Window
{
class PerformerDialog: public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    PerformerDialog(wxWindow* parent,
                    const Antares::Action::Context::Ptr& context,
                    const Antares::Action::IAction::Ptr& root);

    //! Destructor
    virtual ~PerformerDialog();
    //@}

    /*!
    ** \brief Open the actions dialog
    */
    void closeThenOpenActionsDialog();

    /*!
    ** \brief Close the window
    */
    void askForClosingTheWindow();

    void notifyHasBeenModified();

    void notifyProgression(int progress, int max);

    void updateGUI();

private:
    //! Event: The user asked to cancel the operation
    void onCancel(void*);

    void delegateUserInfo(const Yuni::String& text);

    void closeWindow();

private:
    //! Flag to know if the study has been modified
    bool pHasBeenModified;

    //! The target study
    Antares::Action::Context::Ptr pContext;
    //! The tree of the actions to perform
    Antares::Action::IAction::Ptr pActions;
    //! Gauge
    Component::ProgressBar* pGauge;
    //! Cancel button
    wxButton* pBtnCancel;

    wxStaticText* pLblMessage;

    Yuni::String pNextMessage;

    wxTimer* pTimer;

    Yuni::Thread::IThread* pThread;
    std::mutex pMutex;
    std::atomic<int> pGUINeedRefresh;
    std::atomic<int> pProgression;
    std::atomic<int> pProgressionMax;

}; // class PerformerDialog

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_EXT_SOURCE_PERFORMER_H__
