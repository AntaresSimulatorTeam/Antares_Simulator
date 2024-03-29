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

#include "gui.h"
#include <antares/logs/logs.h>
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/timer.h>

using namespace Yuni;

namespace Antares
{
namespace Private
{
namespace Dispatcher
{
class JobLayout final : public Yuni::Job::IJob
{
public:
    explicit JobLayout(wxSizer* sizer) : pSizer(sizer)
    {
        assert(sizer);
    }

    virtual ~JobLayout()
    {
    }

protected:
    virtual void onExecute()
    {
        wxWindow* parent = pSizer->GetContainingWindow();
        if (parent)
        {
            pSizer->Layout();
            parent->Refresh();
        }
    }

private:
    wxSizer* pSizer;
};

class JobRefresh final : public Yuni::Job::IJob
{
public:
    explicit JobRefresh(wxWindow* window) : pWindow(window)
    {
        assert(pWindow);
        if (window)
        {
            auto* info = window->GetClassInfo();
            if (info)
                name = info->GetClassName();
            pWindowID = pWindow->GetId();
        }
    }

    virtual ~JobRefresh()
    {
    }

protected:
    virtual void onExecute()
    {
        if (pWindow && wxWindow::FindWindowById(pWindowID))
            pWindow->Refresh();
    }

private:
    wxWindow* pWindow;
    wxWindowID pWindowID;
    wxString name;
};

class JobShowModalThenDestroy final : public Yuni::Job::IJob
{
public:
    explicit JobShowModalThenDestroy(wxDialog* form, bool destroy) : pForm(form), pDestroy(destroy)
    {
        assert(form);
    }

    virtual ~JobShowModalThenDestroy()
    {
    }

protected:
    virtual void onExecute()
    {
        pForm->ShowModal();
        if (pDestroy)
            pForm->Destroy();
    }

private:
    wxDialog* pForm;
    bool pDestroy;

}; // class JobShowModalThenDestroy

class JobShow final : public Yuni::Job::IJob
{
public:
    explicit JobShow(wxWindow* form, bool focus, bool makeModal) :
     pForm(form), pFocus(focus), pMakeModal(makeModal)
    {
        assert(pForm);
    }

    virtual ~JobShow()
    {
    }

protected:
    virtual void onExecute()
    {
        pForm->Show();
        if (pFocus)
            pForm->SetFocus();
        if (pMakeModal)
            wxWindowDisabler disabler(pForm);
    }

private:
    wxWindow* pForm;
    bool pFocus;
    bool pMakeModal;

}; // class JobShow

} // namespace Dispatcher
} // namespace Private
} // namespace Antares

namespace Antares
{
namespace Dispatcher
{
namespace GUI
{
void Layout(wxSizer* sizer)
{
    if (sizer)
        ::Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobLayout(sizer));
}

void Refresh(wxWindow* window)
{
    if (window)
        ::Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobRefresh(window));
}

void Refresh(wxWindow* window, uint delay)
{
    if (window)
        ::Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobRefresh(window),
          delay);
}

void ShowModal(wxDialog* window, bool destroy)
{
    if (window)
    {
        ::Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobShowModalThenDestroy(
            window, destroy));
    }
}

void Show(wxWindow* window, bool focus, bool makeModal)
{
    if (window)
        ::Antares::Dispatcher::GUI::Post(
          (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobShow(
            window, focus, makeModal));
}

namespace // anonymous
{
class JobWindowClose final : public Yuni::Job::IJob
{
public:
    JobWindowClose(wxWindow* object) : pWindow(object)
    {
        assert(object);
    }
    virtual ~JobWindowClose()
    {
    }

protected:
    virtual void onExecute()
    {
        if (pWindow)
            pWindow->Close();
    }

private:
    wxWindow* pWindow;
};

class JobWindowDestroy final : public Yuni::Job::IJob
{
public:
    JobWindowDestroy(wxWindow* object) : pWindow(object)
    {
        assert(object);
    }
    virtual ~JobWindowDestroy()
    {
    }

protected:
    virtual void onExecute()
    {
        pWindow->Destroy();
    }

private:
    wxWindow* pWindow;
};

class JobTimerDestroy final : public Yuni::Job::IJob
{
public:
    JobTimerDestroy(wxTimer* object) : pTimer(object)
    {
    }
    virtual ~JobTimerDestroy()
    {
    }

protected:
    virtual void onExecute()
    {
        delete pTimer;
    }

private:
    wxTimer* pTimer;
};

} // anonymous namespace

void Close(wxWindow* window)
{
    if (!window)
        logs.debug() << "Impossible to close a NULL top level window";
    else
        ::Antares::Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)new JobWindowClose(window));
}

void Close(wxWindow* window, uint delay)
{
    if (!window)
        logs.debug() << "Impossible to close a NULL top level window";
    else
        ::Antares::Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)new JobWindowClose(window),
                                         delay);
}

void Destroy(wxWindow* window)
{
    if (!window)
        logs.debug() << "Impossible to destroy a NULL top level window";
    else
        ::Antares::Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)new JobWindowDestroy(window));
}

void Destroy(wxTimer* timer)
{
    if (!timer)
        logs.debug() << "Impossible to destroy a NULL timer";
    else
    {
        ::Antares::Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)new JobTimerDestroy(timer));
    }
}

} // namespace GUI
} // namespace Dispatcher
} // namespace Antares
