// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <sstream>

#include "port.h"

namespace Antares::ModelerStudy::SystemModel
{

class Component;

class ConnectionEnd final
{
public:
    ConnectionEnd(Component* component, const Port* port);
    [[nodiscard]] const Component* component() const;
    [[nodiscard]] const Port* port() const;

private:
    const Component* component_;
    const Port* port_;
};

} // namespace Antares::ModelerStudy::SystemModel
