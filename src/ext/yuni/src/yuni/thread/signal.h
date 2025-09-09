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
#include "../yuni.h"
#include "../core/smartptr.h"
#include "pthread.h"

namespace Yuni::Thread
{
/*!
** \brief Mecanism for notifying a waiting thread of the occurrence of a particular event
*/
class YUNI_DECL Signal final
{
public:
    //! Most suitable smart pointer for the class
    typedef Yuni::SmartPtr<Signal> Ptr;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Signal();
    //! Copy constructor
    Signal(const Signal&);
#ifdef YUNI_HAS_CPP_MOVE
    // Like mutexes, a signal must have invariant address and thus can not be moved
    Signal(Signal&&) = delete;
#endif
    //! Destructor
    ~Signal();
    //@}

    //! \name Signal
    //@{
    /*!
    ** \brief Reset the internal state
    */
    bool reset();

    /*!
    ** \brief Wait for being notified
    */
    void wait();

    /*!
    ** \brief Wait for being notified
    **
    ** \param timeout A timeout, in milliseconds
    ** \return True if the signal has been notified, false if the timeout has been reached
    */
    bool wait(uint timeout);

    /*!
    ** \brief Wait for being notified and atomically reset for reuse
    */
    void waitAndReset();

    /*!
    ** \brief Notify the waiter
    */
    bool notify();

    /*!
    ** \brief Get if the signal is valid
    */
    bool valid() const;
    //@}

    //! \name Operators
    //@{
    //! Operator !
    bool operator!() const;
    //! Operator =
    Signal& operator=(const Signal&);

#ifdef YUNI_HAS_CPP_MOVE
    // Like mutexes, a signal must have invariant address and thus can not be moved
    Signal& operator=(Signal&&) = delete;
#endif

    //@}

private:
#ifndef YUNI_NO_THREAD_SAFE
#ifdef YUNI_OS_WINDOWS
    //! Event handle (HANDLE)
    void* pHandle;
#else
    pthread_mutex_t pMutex;
    pthread_cond_t pCondition;
    volatile bool pSignalled;
#endif
#endif

}; // class Signal

} // namespace Yuni::Thread

#include "signal.hxx"
