// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LOGS_CLEANER_H__
#define __ANTARES_LOGS_CLEANER_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>

namespace Antares
{
enum
{
    defaultRetentionTime = (60 * 60 * 24) * 31,
};

/*!
** \brief Purge all log files
*/
void PurgeLogFiles(const AnyString& path, uint retention = defaultRetentionTime);

} // namespace Antares

#endif // __ANTARES_LOGS_CLEANER_H__
