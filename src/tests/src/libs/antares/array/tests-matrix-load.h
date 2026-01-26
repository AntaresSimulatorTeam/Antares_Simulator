// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <global_JIT_manager.h>

#include "matrix-bypass-load.h"

enum
{
    //! A Hard-coded maximum filesize
    filesizeHardLimit = 1536 * 1024 * 1024, // 1.5Go
};
