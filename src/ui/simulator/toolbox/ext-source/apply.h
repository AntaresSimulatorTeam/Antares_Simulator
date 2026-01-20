// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__

#include <yuni/yuni.h>
#include <antares/study/study.h>
#include <action/action.h>
#include <action/context.h>

namespace Antares::ExtSource::Handler
{
/*!
** \brief Apply all actions
*/
void Apply(Antares::Action::Context::Ptr context,
           Antares::Action::IAction::Ptr actions,
           bool windowRequired = false);

} // namespace Antares::ExtSource::Handler

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_APPLY_H__
