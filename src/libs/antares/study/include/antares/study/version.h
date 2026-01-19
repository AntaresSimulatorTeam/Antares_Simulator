// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
class StudyVersion final
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
