// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/io/inputs/yml-utils/DiagnosticsScope.h"

#include <utility>

#include <antares/io/inputs/InputError.h>

#include "antares/logs/logs.h"

namespace Antares::IO::Inputs::YmlUtils
{

namespace
{
thread_local DiagnosticsScope* currentScope = nullptr;
} // namespace

DiagnosticsScope::DiagnosticsScope():
    previous_(currentScope)
{
    currentScope = this;
}

DiagnosticsScope::~DiagnosticsScope()
{
    currentScope = previous_;
    logWarnings();
}

DiagnosticsScope* DiagnosticsScope::current()
{
    return currentScope;
}

void DiagnosticsScope::addError(std::string message)
{
    errors_.push_back(std::move(message));
}

void DiagnosticsScope::addWarning(std::string message)
{
    warnings_.push_back(std::move(message));
}

void DiagnosticsScope::logWarnings()
{
    for (const auto& warning: warnings_)
    {
        Antares::logs.warning() << warning;
    }
    warnings_.clear();
}

void DiagnosticsScope::flush()
{
    logWarnings();
    if (errors_.empty())
    {
        return;
    }

    std::string all;
    for (const auto& error: errors_)
    {
        all += error;
        all += '\n';
    }
    errors_.clear();
    throw InputError(all);
}

} // namespace Antares::IO::Inputs::YmlUtils
