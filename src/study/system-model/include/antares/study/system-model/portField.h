// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

namespace Antares::ModelerStudy::SystemModel
{

class PortField
{
public:
    explicit PortField(const std::string& id):
        id_(id)
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    bool operator==(const PortField&) const = default;

    bool operator<(const PortField& other) const
    {
        return id_ < other.id_;
    }

private:
    std::string id_;
};

} // namespace Antares::ModelerStudy::SystemModel
