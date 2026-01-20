// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <yuni/yuni.h>
#include <action/state.h>
#include <cassert>

namespace Antares::Action
{
const char* StateToString(State state)
{
    static const char* const str[] = {
      "unknown",
      "disabled",
      "error",
      "nothing to do",
      "ready",
      "conflict",
    };
    assert((uint)state < (uint)stMax);
    return str[(uint)state];
}

} // namespace Antares::Action
