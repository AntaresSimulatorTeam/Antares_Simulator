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
#ifndef __ANTARES_APPLICATION_MAIN_DISABLER_H__
#define __ANTARES_APPLICATION_MAIN_DISABLER_H__

namespace Antares
{
namespace Forms
{
template<class WindowT>
class Disabler final
{
public:
    Disabler(WindowT& p) : pWindow(p)
    {
        if (!pWindow.pUpdateCountLocker++)
            pWindow.Enable(false);
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
    Disabler(ApplWnd& p) : pWindow(p)
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

} // namespace Forms
} // namespace Antares

#endif // __ANTARES_APPLICATION_MAIN_DISABLER_H__
