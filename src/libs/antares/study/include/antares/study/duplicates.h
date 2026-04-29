// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Data
{
// Forward decl, we don't need the full definition for Study
class Study;
} // namespace Antares::Data

namespace Antares::Check
{
/* Check for duplicate elements
   - For each area
    - Thermal clusters
    - Renewable clusters
    - Short-term storage
   - Binding constraints
   Return true if no duplicates, false if some duplicates
*/
bool checkForDuplicates(const Antares::Data::Study& study);
} // namespace Antares::Check
