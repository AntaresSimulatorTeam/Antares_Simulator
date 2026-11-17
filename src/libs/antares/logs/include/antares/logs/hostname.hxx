// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/logs/hostinfo.h"

namespace // anonymous
{
//! Appends the host name to any string-like object accepting operator << (const char*)
template<class AnyStringT>
void InternalAppendHostname(AnyStringT& out)
{
    out << Antares::Logs::hostName().c_str();
}

} // anonymous namespace
