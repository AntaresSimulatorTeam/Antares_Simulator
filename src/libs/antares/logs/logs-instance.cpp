// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/logs/logs.h"

// Kept in its own translation unit, separate from Logger/LogBuffer, so that pulling in the
// LogBuffer implementation from the static library does not also drag in this definition of
// Antares::logs. Test binaries that provide their own fake Antares::logs (see
// src/tests/src/libs/antares/logs) rely on that separation to avoid a duplicate symbol at link
// time.
namespace Antares
{
Logs::Logger logs;
}
