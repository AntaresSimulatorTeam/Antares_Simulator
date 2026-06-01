// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#include <chrono>

#include <antares/benchmarking/timer.h>

static inline int64_t MilliSecTimer()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
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
