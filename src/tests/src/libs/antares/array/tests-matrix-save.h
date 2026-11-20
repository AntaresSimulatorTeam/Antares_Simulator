// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <cstdint>

#include <antares/array/matrix-io.h>

#include "fill-matrix.h"
using namespace std;
using namespace Antares;

struct PredicateIdentity
{
    template<class U>
    inline U operator()(const U& value) const
    {
        return value;
    }
};
