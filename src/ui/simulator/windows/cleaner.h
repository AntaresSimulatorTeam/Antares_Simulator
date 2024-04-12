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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_APPLICATION_WINDOW_CLEANER_H__
#define __ANTARES_APPLICATION_WINDOW_CLEANER_H__

#include <mutex>
#include <yuni/thread/thread.h>
#include <yuni/thread/mutex.h>
#include <wx/panel.h>
#include <wx/dialog.h>
#include "../toolbox/components/htmllistbox/component.h"
#include <antares/study/cleaner.h>
#include <wx/stattext.h>
#include <wx/button.h>

namespace Antares
{
namespace Window
{
// Forward declaration
class CleaningThread;

class StudyCleaner final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor, with a parent frame
    */
    StudyCleaner(wxFrame* parent);
    //! Destructor
    virtual ~StudyCleaner();
    //@}

    //! \name Folder
    //@{
    //! Set the study folder
    void studyFolder(const wxString& folder);
    //! Get the study folder
    const wxString& studyFolder() const
    {
        return pFolder;
    }
    //@}

    //! \name Progression
    //@{
    void progress(uint count);
    void updateProgressionLabel();
    //@}

private:
    void onRefresh(void*);
    void onCancel(void*);
    void onProceed(void*);
    void updateGUI(bool hasItems);
    void launchCleanup();
    void closeMe();

private:
    //! The study folder
    wxString pFolder;
    //! Listbox
    Component::HTMLListbox::Component* pListbox;
    //!
    Data::StudyCleaningInfos* pInfos;
    //!
    wxStaticText* pLblInfos;
    wxButton* pBtnRefresh;
    wxButton* pBtnCancel;
    wxButton* pBtnGo;

    wxTimer* pRefreshTimer;
    Yuni::Thread::IThread* pThread;
    std::mutex pMutex;
    uint pProgressionCount;

    friend class CleaningThread;

}; // class StudyCleaner

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_CLEANER_H__
