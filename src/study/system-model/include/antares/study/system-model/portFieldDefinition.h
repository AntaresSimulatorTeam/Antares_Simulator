// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "port.h"
#include "portType.h"

namespace Antares::ModelerStudy::SystemModel
{

class PortFieldDefinition final
{
public:
    PortFieldDefinition(Port port, PortField field, Expression definition):
        port_(std::move(port)),
        field_(std::move(field)),
        definition_(std::move(definition))
    {
    }

    const Port& port() const
    {
        return port_;
    }

    const PortField& field() const
    {
        return field_;
    }

    const Expression& Definition() const
    {
        return definition_;
    }

private:
    Port port_;
    PortField field_;
    Expression definition_;
};

} // namespace Antares::ModelerStudy::SystemModel
