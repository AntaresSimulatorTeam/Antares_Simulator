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
            // restart the time in background, used to flush from time
            // to time the memory to their swap files
            pWindow.backgroundTimerStart();
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
