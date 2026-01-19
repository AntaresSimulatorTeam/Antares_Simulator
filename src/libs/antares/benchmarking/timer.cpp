// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <yuni/core/system/gettimeofday.h>

#include <antares/benchmarking/timer.h>

using namespace Yuni;

static inline int64_t MilliSecTimer()
{
    Yuni::timeval tv;
    YUNI_SYSTEM_GETTIMEOFDAY(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

namespace Benchmarking
{
Timer::Timer()
{
    startTime_ = MilliSecTimer();
}

void Timer::stop()
{
    duration_ = MilliSecTimer() - startTime_;
}

int64_t Timer::get_duration()
{
    return duration_;
}

} // namespace Benchmarking
