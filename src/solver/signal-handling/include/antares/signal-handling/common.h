// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include <antares/writer/i_writer.h>

void signalCtrl_term(int);
void signalCtrl_int(int);

namespace Antares::Solver
{
/*!
 * Defines the provided writer as the unique writer managed by the running application.
 */
void setApplicationResultWriter(std::weak_ptr<IResultWriter> writer);

} // namespace Antares::Solver
