// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __SOLVER_MISC_SYSTEM_MEMORY_H__
#define __SOLVER_MISC_SYSTEM_MEMORY_H__

#include <yuni/yuni.h>
#include <yuni/thread/timer.h>

class SystemMemoryLogger final: public Yuni::Thread::Timer
{
public:
    SystemMemoryLogger();
    ~SystemMemoryLogger() override;

protected:
    bool onInterval(uint) override;

    bool onStarting() override;

}; // class SystemMemoryLogguer

#endif /* __SOLVER_MISC_SYSTEM_MEMORY_H__ */
