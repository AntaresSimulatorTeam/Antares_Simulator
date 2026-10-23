// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>
#include <vector>

namespace Antares::IO::Inputs::YmlUtils
{

/// While alive on the current thread, checkFields records its findings here instead of throwing,
/// so that a whole file can be parsed and all errors and warnings reported at once.
/// Missing and duplicated fields are errors, unexpected fields are warnings.
class DiagnosticsScope
{
public:
    DiagnosticsScope();
    ~DiagnosticsScope();

    DiagnosticsScope(const DiagnosticsScope&) = delete;
    DiagnosticsScope& operator=(const DiagnosticsScope&) = delete;

    /// The innermost active scope of the current thread, or nullptr.
    static DiagnosticsScope* current();

    void addError(std::string message);
    void addWarning(std::string message);

    /// Logs the pending warnings, then throws InputError gathering all pending errors, if any.
    void flush();

private:
    void logWarnings();

    DiagnosticsScope* previous_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace Antares::IO::Inputs::YmlUtils
