// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_HXX__

namespace Antares::Data
{
inline void PartRenewable::resizeAllTimeseriesNumbers(uint n)
{
    list.resizeAllTimeseriesNumbers(n);
}

} // namespace Antares::Data

#endif // __ANTARES_LIBS_STUDY_PARTS_RENEWABLE_CONTAINER_HXX__
