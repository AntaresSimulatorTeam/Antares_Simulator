// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#pragma once

namespace Antares::Data
{
namespace accessForTests // anonymous
{
bool loadReservesParameters(fs::path& folderInput, Area& area);

void validateCapacityReservations(const Area& area);
} // namespace accessForTests
} // namespace Antares::Data
