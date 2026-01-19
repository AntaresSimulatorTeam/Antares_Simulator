// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

namespace Antares::Solver
{
class ILoader;
class IWriter;

class Modeler final
{
public:
    Modeler(ILoader& loader, IWriter& writer);
    void run() const;

    class ModelerError: public std::runtime_error
    {
    public:
        explicit ModelerError(const std::string& s):
            runtime_error(s)
        {
        }
    };

    ILoader& loader_;
    IWriter& writer_;
};
} // namespace Antares::Solver
