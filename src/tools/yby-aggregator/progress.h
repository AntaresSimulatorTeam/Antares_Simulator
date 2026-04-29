// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __STUDY_JOB_AGGREGATOR_PROGRESS_H__
#define __STUDY_JOB_AGGREGATOR_PROGRESS_H__

#include <atomic>

#include <yuni/thread/timer.h>

#include <antares/logs/logs.h>

class Progress final: public Yuni::Thread::Timer
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
    static std::atomic<int> Current;
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
