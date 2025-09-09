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
#include "program.h"
#include "../../noncopyable.h"
#include "../../../thread/thread.h"
#ifdef YUNI_OS_UNIX
#include <sys/types.h>
#endif
#include <signal.h>

namespace Yuni::Process
{
/*!
** \brief Helper class for storing shared information on the program which is currently
** launched by Process::Program
**
** \note This class may be shared by several threads
*/
class Program::ProcessSharedInfo final: public Yuni::NonCopyable<Program::ProcessSharedInfo>
{
public:
    //! Smart pointer
    // \note This type must match the definition of \p pEnv
    typedef SmartPtr<ProcessSharedInfo> Ptr;

public:
    //! \name Constructor & Destructor
    //@{
    //! Default constructor
    ProcessSharedInfo();
    //! Destructor
    ~ProcessSharedInfo();
    //@}

    /*!
    ** \return True if the signal has been delivered
    */
    template<bool WithLock>
    bool sendSignal(int value);

    /*!
    ** \brief Create a thread dedicated to handle the execution timeout
    */
    void createThreadForTimeoutWL();

public:
    String executable;
    //! The command
    String::Vector arguments;
    //! The working directory
    String workingDirectory;
    //! Flag to know if the process is running
    bool running;
    //! PID
    int processID;
    //! input file descriptors
    int processInput;
    //! Thread
    ThreadPtr thread;
    //! Duration in seconds
    int64_t duration;
    //! Duration precision
    DurationPrecision durationPrecision;
    //! Timeout
    uint timeout;
    //! Exit status
    int exitstatus;
    //! Console
    bool redirectToConsole;
    //! Mutex
    mutable Mutex mutex;

    //! Optional thread for timeout
    Yuni::Thread::IThread* timeoutThread;

}; // class Program::ProcessSharedInfo

} // namespace Yuni::Process

#include "process-info.hxx"
