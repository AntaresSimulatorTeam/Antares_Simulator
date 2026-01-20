// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <stdexcept>

#include <antares/study/system-model/connection.h>

namespace Antares::ModelerStudy::SystemModel
{

ConnectionEnd::ConnectionEnd(Component* component, const Port* port)
{
    if (component == nullptr)
    {
        throw std::invalid_argument("component must not be null");
    }
    if (port == nullptr)
    {
        throw std::invalid_argument("port must not be null");
    }

    component_ = component;
    port_ = port;
}

const Component* ConnectionEnd::component() const
{
    return component_;
}

const Port* ConnectionEnd::port() const
{
    return port_;
}

} // namespace Antares::ModelerStudy::SystemModel
