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
#ifndef __STUDY_JOB_AGGREGATOR_PROGRESS_H__
#define __STUDY_JOB_AGGREGATOR_PROGRESS_H__

#include <yuni/yuni.h>
#include <yuni/thread/timer.h>
#include <yuni/core/atomic/int.h>
#include <antares/logs.h>

class Progress final : public Yuni::Thread::Timer
{
public:
    //! The threading policy
    using ThreadingPolicy = Yuni::Thread::Timer::ThreadingPolicy;
    //! Different states related to the progression
    enum State
    {
        stJobs,
        stWrite,
        stReading,
        stSilent,
    };

public:
    //! The number of jobs currently done
    static Yuni::Atomic::Int<> Current;
    //! The total number of jobs to execute
    static uint Total;

public:
    Progress();
    virtual ~Progress();

    /*!
    ** \brief Get if the task is complete
    */
    bool completed() const;

    //! Set the message to display into logs
    template<class StringT>
    void message(const StringT& msg);

public:
    //! The current state in the progression
    State state;

protected:
    virtual bool onInterval(uint) override;

private:
    //! Flag to know if the task is complete
    bool pCompleted;
    //! The message to display into logs
    Yuni::String pMessage;
    //! Last value computed for the percent
    uint pLastPercent;

}; // class Progress

extern Progress progressBar;

#include "progress.hxx"

#endif // __STUDY_JOB_AGGREGATOR_PROGRESS_H__
