// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_WINDOWS_HYDRO_ALLOCATION_H__
#define __ANTARES_APPLICATION_WINDOWS_HYDRO_ALLOCATION_H__

#include <ui/common/component/panel.h>

namespace Antares::Window::Hydro
{
class Allocation: public Component::Panel
{
public:
    //! name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Allocation(wxWindow* parent);
    //! Destructor
    virtual ~Allocation();
    //@}

}; // class Series

} // namespace Antares::Window::Hydro

#endif // __ANTARES_APPLICATION_WINDOWS_HYDRO_ALLOCATION_H__
