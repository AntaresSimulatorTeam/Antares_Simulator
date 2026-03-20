// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cstdint>
#include <global_JIT_manager.h>

#include "fill-matrix.h"
#include "matrix-bypass-load.h"
using namespace std;
using namespace Yuni;
using namespace Antares;

struct TSNumbersPredicate
{
    uint32_t operator()(uint32_t value) const
    {
        return value + 1;
    }
};

struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};
