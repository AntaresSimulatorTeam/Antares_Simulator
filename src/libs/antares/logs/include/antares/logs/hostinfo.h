// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_HOST_INFO_H__
#define __ANTARES_LIBS_HOST_INFO_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

/*!
** \brief Write the host info into logs
*/
void WriteHostInfoIntoLogs();

void AppendHostName(Yuni::String& out);

#endif // __ANTARES_LIBS_HOST_INFO_H__
