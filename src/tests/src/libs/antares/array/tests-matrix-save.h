// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cstdint>

#include "fill-matrix.h"
#include "matrix-bypass-load.h"
using namespace std;
using namespace Yuni;
using namespace Antares;

struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};
