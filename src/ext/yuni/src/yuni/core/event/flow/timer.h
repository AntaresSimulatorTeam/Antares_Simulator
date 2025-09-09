/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include "../../../yuni.h"
#include "../../../thread/thread.h"
#include <cassert>

namespace Yuni::Core::EventLoop::Flow
{
template<class EventLoopT>
class YUNI_DECL Timer
{
public:
    //! Type of the event loop
    typedef EventLoopT EventLoopType;

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Timer():
        pTimeout(200),
        pEventLoop(NULL)
    {
    }

    //@}

    /*!
    ** \brief Set the timeout to wait between each cycle
    **
    ** \note Due to implementation limitations, the timeout is limited to 10s when
    ** the loop is not in detached mode. There is no restriction in detached mode
    ** (which is the default).
    */
    void timeout(uint t)
    {
        if (!EventLoopType::detached)
        {
            // Hard limit when not in detached mode.
            // When not in detached mode, there is no cancellation point
            // so it is impossible to properly stop the loop if the timeout is too high
            if (t > 10000)
            {
                t = 10000;
            }
        }
        // The timeout will be set from the loop to avoid continuous locking
        Bind<bool()> b;
        b.bind(this, &Timer::internalSetLoopTimeout, t);
        pEventLoop->dispatch(b);
    }

protected:
    //! \name Events triggered by the public interface of the event loop (from any thread)
    //@{
    /*!
    ** \brief The event loop has just started
    **
    ** The event loop is locked when this method is called
    */
    static bool onStart()
    {
        return true;
    }

    /*!
    ** \brief The event loop has just stopped
    **
    ** The event loop is locked when this method is called
    */
    static bool onStop()
    {
        return true;
    }

    /*!
    ** \brief A new request has just been added into the queue
    **
    ** The event loop is locked when this method is called
    ** \param request The request (bind, see EventLoopType::RequestType)
    ** \return True to allow the request to be posted
    */
    template<class U>
    static bool onRequestPosted(const U& request)
    {
        (void)request;
        return true;
    }

    //@}

    //! \name Events triggered from the main thread of the event loop
    //@{
    /*!
    ** \brief The event loop has started a new cycle
    **
    ** This method is called from the main thread of the event loop.
    ** No lock is provided.
    */
    bool onNewCycle()
    {
        assert(pEventLoop);
        pEventLoop->suspend(pTimeout);
        return true;
    }

    //@}

    /*!
    ** \brief Event triggered from the constructor of the event loop
    ** \param e Pointer to the original event loop
    */
    void onInitialize(EventLoopType* e)
    {
        pEventLoop = e;
    }

private:
    bool internalSetLoopTimeout(uint t)
    {
        // We assume here that `t` is valid.
        pTimeout = t;
        return true;
    }

private:
    //! Timeout (ms)
    uint pTimeout;
    //! Pointer to the original event loop
    EventLoopType* pEventLoop;

}; // class Timer<>

} // namespace Yuni::Core::EventLoop::Flow
