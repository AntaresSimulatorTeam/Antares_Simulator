// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/expressions/visitors/NodeVisitor.h>
#include <antares/logs/logs.h>

namespace Antares::Expressions::Visitors
{
void logError(const std::string& msg)
{
    logs.error() << msg;
}
} // namespace Antares::Expressions::Visitors
