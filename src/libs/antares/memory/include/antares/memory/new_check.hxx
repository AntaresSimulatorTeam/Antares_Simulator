// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <memory>

#include <antares/logs/logs.h>

namespace Antares::MemoryUtils
{
template<class T, class... Args>
T* new_check_allocation(Args... args)
{
    auto r = new (std::nothrow) T(args...);
    if (r == nullptr)
    {
        logs.error() << "Error allocating memory";
    }
    return r;
}
} // namespace Antares::MemoryUtils
