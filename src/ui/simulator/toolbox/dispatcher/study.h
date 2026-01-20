// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_DISPATCHER_STUDY_H__
#define __ANTARES_TOOLBOX_DISPATCHER_STUDY_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares::Dispatcher
{
/*!
** \brief Create a new study
*/
void StudyNew(bool force = false);

/*!
** \brief Open a study
*/
void StudyOpen(const Yuni::String& folder, bool force = false);

/*!
** \brief Close the study
*/
void StudyClose(bool force = false, bool closeMainFrm = false);

} // namespace Antares::Dispatcher

#endif // __ANTARES_TOOLBOX_DISPATCHER_STUDY_H__
