// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_MAIN_DISABLER_H__
#define __ANTARES_APPLICATION_MAIN_DISABLER_H__

namespace Antares::Forms
{
template<class WindowT>
class Disabler final
{
public:
    Disabler(WindowT& p):
        pWindow(p)
    {
        if (!pWindow.pUpdateCountLocker++)
        {
            pWindow.Enable(false);
        }
    }

    ~Disabler()
    {
        assert(pWindow.pUpdateCountLocker > 0);
        if (!(--pWindow.pUpdateCountLocker))
        {
            pWindow.Enable(true);
            pWindow.SetFocus();
        }
    }

private:
    WindowT& pWindow;
};

template<>
class Disabler<ApplWnd> final
{
public:
    Disabler(ApplWnd& p):
        pWindow(p)
    {
        if (!pWindow.pUpdateCountLocker++)
        {
            // disable the background timer
            pWindow.backgroundTimerStop();
            // disable the drag-drop target
            pWindow.SetDropTarget(nullptr);
        }
    }

    ~Disabler()
    {
        assert(pWindow.pUpdateCountLocker > 0);
        if (!(--pWindow.pUpdateCountLocker))
        {
            // resetting the text of the status bar
            pWindow.resetDefaultStatusBarText();
            // The Form may have lost its focus
            // This action must be delayed
            pWindow.delayForceFocus();
            // restore the drag-drop
            pWindow.resetDragAndDrop();
        }
    }

private:
    //! Alias to the current window
    ApplWnd& pWindow;
};

} // namespace Antares::Forms

#endif // __ANTARES_APPLICATION_MAIN_DISABLER_H__
