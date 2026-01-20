// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include "portType.h"

namespace Antares::ModelerStudy::SystemModel
{

class Port final
{
public:
    Port(const std::string& id, const PortType& type):
        id_(id),
        type_(type)
    {
    }

    const std::string& Id() const
    {
        return id_;
    }

    const PortType& Type() const
    {
        return type_;
    }

private:
    std::string id_;
    PortType type_;
};

} // namespace Antares::ModelerStudy::SystemModel
