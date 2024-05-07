/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <array>
#include <string>

namespace Antares::Data
{
/*!
** \brief Version of a study
**
** \ingroup study
** \see CHANGELOG.txt
*/
class StudyVersion
{
public:
    /// allows automatic members comparison
    auto operator<=>(const StudyVersion&) const = default;

    constexpr StudyVersion() = default;

    constexpr StudyVersion(unsigned major, unsigned minor):
        major_(major),
        minor_(minor)
    {
    }

    ~StudyVersion() = default;

    bool isSupported(bool verbose) const;

    bool fromString(const std::string& versionStr);

    std::string toString() const;

    static StudyVersion latest();
    static StudyVersion unknown();

private:
    unsigned major_ = 0;
    unsigned minor_ = 0;
};
} // namespace Antares::Data
