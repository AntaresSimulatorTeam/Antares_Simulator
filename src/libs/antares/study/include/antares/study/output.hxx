// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_OUTPUT_HXX__
#define __ANTARES_LIBS_STUDY_OUTPUT_HXX__

namespace Antares::Data
{
inline bool Output::operator<(const Output& rhs) const
{
    return not(title < rhs.title);
}
} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_OUTPUT_HXX__
