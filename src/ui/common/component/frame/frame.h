// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__

#include <yuni/yuni.h>
#include <yuni/string.h>
#include <vector>

namespace Antares::Component::Frame
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

} // namespace Antares::Component::Frame

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_FRAME_H__
