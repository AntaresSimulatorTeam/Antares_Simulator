// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_SAVEAS_HXX__
#define __ANTARES_APPLICATION_WINDOWS_SAVEAS_HXX__

namespace Antares::Window
{
inline Data::Study::Ptr SaveAs::study()
{
    return pStudy;
}

inline const Data::Study::Ptr SaveAs::study() const
{
    return pStudy;
}

inline SaveResult SaveAs::result() const
{
    return pResult;
}

} // namespace Antares::Window

#endif // __ANTARES_APPLICATION_WINDOWS_SAVEAS_HXX__
