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

#include "antares/study/study.h"
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../../common/lock.h"
#include "study.h"

using namespace Yuni;

namespace Antares
{
namespace Dispatcher
{
namespace // anonymous
{
class JobStudyClose : public Yuni::Job::IJob
{
public:
    JobStudyClose(bool force, bool closeMainFrm) : pForce(force), pQuit(closeMainFrm)
    {
    }
    virtual ~JobStudyClose()
    {
    }

protected:
    virtual void onExecute()
    {
        Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
        if (CurrentStudyIsValid())
        {
            if (pForce || mainFrm->wouldYouLikeToSaveTheStudy())
                ::Antares::CloseTheStudy(true);
            else
            {
                GUIIsNoLongerQuitting();
                // mainFrm->Enable(true);
            }
        }
        else
        {
            if (pQuit)
                Dispatcher::GUI::Close(mainFrm);
        }
    }

private:
    bool pForce;
    bool pQuit;
};

class JobStudyNew : public Yuni::Job::IJob
{
public:
    JobStudyNew(bool force) : pForce(force)
    {
    }
    virtual ~JobStudyNew()
    {
    }

protected:
    virtual void onExecute()
    {
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
        if (pForce || !StudyHasBeenModified() || mainFrm.wouldYouLikeToSaveTheStudy())
        {
            GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyClose(true, false));
            Bind<void()> callback;
            callback.bind(&::Antares::NewStudy);
            Dispatcher::GUI::Post(callback);
        }
    }

private:
    bool pForce;
};

} // anonymous namespace

void StudyNew(bool force)
{
    GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyNew(force), 50);
}

void StudyClose(bool force, bool closeMainFrm)
{
    if (force)
        GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyClose(true, closeMainFrm));
    else
        GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyClose(false, closeMainFrm), 80);
}

namespace // anonymous
{
class JobStudyOpen : public Yuni::Job::IJob
{
public:
    JobStudyOpen(const String& folder) : pFolder(folder)
    {
    }
    virtual ~JobStudyOpen()
    {
    }

protected:
    virtual void onExecute()
    {
        ::Antares::OpenStudyFromFolder(wxStringFromUTF8(pFolder));
    }

private:
    const String pFolder;
};

class JobStudyCloseThenOpen : public Yuni::Job::IJob
{
public:
    JobStudyCloseThenOpen(const String& folder) : pFolder(folder)
    {
    }
    virtual ~JobStudyCloseThenOpen()
    {
    }

protected:
    virtual void onExecute()
    {
        Forms::ApplWnd& mainFrm = *Forms::ApplWnd::Instance();
        if (mainFrm.wouldYouLikeToSaveTheStudy())
        {
            CloseTheStudy();
            // Open the study as soon as possible
            mainFrm.SetStatusText(wxString() << wxT("  Loading ") << wxStringFromUTF8(pFolder));
            mainFrm.Refresh();
            GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyOpen(pFolder), 50);
        }
    }

private:
    const String pFolder;
};

} // anonymous namespace

void StudyOpen(const String& folder, bool force)
{
    if (force || not CurrentStudyIsValid() || !StudyHasBeenModified())
    {
        // Close the study first
        StudyClose(true);
        // Open the study as soon as possible
        GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyOpen(folder), 50);
    }
    else
        GUI::Post((const Yuni::Job::IJob::Ptr&)new JobStudyCloseThenOpen(folder), 50);
}

} // namespace Dispatcher
} // namespace Antares
