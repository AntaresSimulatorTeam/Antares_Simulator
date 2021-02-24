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
#ifndef __ANTARES_APPLICATION_WINDOW_CLEANER_H__
#define __ANTARES_APPLICATION_WINDOW_CLEANER_H__

#include <antares/wx-wrapper.h>
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
    Yuni::Mutex pMutex;
    uint pProgressionCount;

    friend class CleaningThread;

}; // class StudyCleaner

} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOW_CLEANER_H__
