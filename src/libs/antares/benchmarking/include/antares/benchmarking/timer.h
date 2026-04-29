// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__
#define __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__

#include <cstdint>

namespace Benchmarking
{
class Timer final
{
public:
    Timer();
    void stop();
    int64_t get_duration();

private:
    int64_t startTime_ = 0;
    int64_t duration_ = 0;
};
} // namespace Benchmarking

#endif // __ANTARES_LIBS_TIME_ELAPSED__TIME_ELAPSED_H__
