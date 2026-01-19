// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__
#define __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__

#include "frame.h"

namespace Antares::Component::Frame::Registry
{
/*!
** \brief Register a new frame
*/
void RegisterFrame(IFrame* frame);

/*!
** \brief Unregister a frame
*/
void UnregisterFrame(IFrame* frame);

/*!
** \brief Update all registered frames
*/
void DispatchUpdate();

/*!
** \brief Update all registered frames
**
** param exclude Any frame which should be excluded from the update
*/
void DispatchUpdate(IFrame* exclude);

/*!
** \brief Close all local registered frames
*/
void CloseAllLocal();

/*!
** \brief Get the list of all registered frames
*/
const IFrame::Vector& List();

} // namespace Antares::Component::Frame::Registry

#endif // __ANTARES_COMMON_COMPONENTS_FRAME_REGISTRY_H__
