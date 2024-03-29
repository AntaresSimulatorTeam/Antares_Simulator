/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__

#include <yuni/yuni.h>
#include <yuni/string.h>
#include <vector>

namespace Antares
{
namespace Component
{
namespace Frame
{
/*!
** \brief Interface for all registered frames, external or not
*/
class IFrame
{
public:
    //! Vector
    using Vector = std::vector<IFrame*>;

public:
    /*!
    ** \brief Raise the frame to the top level
    */
    virtual void frameRaise() = 0;

    /*!
    ** \brief Get the frame title
    */
    virtual Yuni::String frameTitle() const = 0;

    /*!
    ** \brief
    */
    virtual bool excludeFromMenu() = 0;
    virtual void updateOpenWindowsMenu() = 0;
    virtual int frameID() const = 0;

}; // class IFrame

} // namespace Frame
} // namespace Component
} // namespace Antares

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__
