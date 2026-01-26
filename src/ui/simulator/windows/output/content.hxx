// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__
#define __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__

namespace Antares::Private::OutputViewerData
{
inline Content::Content():
    hasYearByYear(false),
    hasConcatenedYbY(false)
{
    ybyInterval[0] = (uint)-1;
    ybyInterval[1] = 0;
}

inline Content::~Content()
{
}

inline bool Content::SimulationType::empty() const
{
    return areas.empty() && links.empty();
}

inline bool Content::empty() const
{
    return economy.empty() && adequacy.empty();
}

} // namespace Antares::Private::OutputViewerData

#endif // __ANTARES_WINDOWS_OUTPUT_CONTENT_HXX__
