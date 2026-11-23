// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <antares/study/system-model/connection.h>

namespace Antares::ModelerStudy::SystemModel
{

ConnectionEnd::ConnectionEnd(Component& component, const Port& port):
    component_(component),
    port_(port)
{
}

const Component& ConnectionEnd::component() const
{
    return component_;
}

const Port& ConnectionEnd::port() const
{
    return port_;
}

} // namespace Antares::ModelerStudy::SystemModel
