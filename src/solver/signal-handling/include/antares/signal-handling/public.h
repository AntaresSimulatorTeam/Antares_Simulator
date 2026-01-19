// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include "antares/writer/i_writer.h"

namespace Antares::Solver
{
void initializeSignalHandlers(std::weak_ptr<IResultWriter> writer);
}
