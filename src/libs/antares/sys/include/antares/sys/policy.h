// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ADMIN_POLICY_H__
#define __ANTARES_LIBS_ADMIN_POLICY_H__

#include <yuni/yuni.h>
#include <yuni/core/singleton.h>
#include <yuni/core/string.h>

/*!
** \brief Local policy settings
*/
namespace LocalPolicy
{
//! A key entry
using PolicyKey = Yuni::ShortString64;

/*!
** \brief Open the local policies
**
** Nothing will be done (and will return true) if the local policies are already
** loaded.
** \param expandEntries True to expand the raw values
** \return True if some local policies have been found
*/
bool Open(bool expandEntries = true);

/*!
** \brief Close all resources related to the local policies
*/
void Close();

/*!
** \brief Check the root prefix, and throws a FatalError if fails.
*/
void CheckRootPrefix(const char* argv0);

/*!
** \brief Read the content of a single entry
**
** If the local policies are not opened, the value is guaranted to be empty.
** \return True if the entry exists, false otherwise
*/
bool Read(YString& out, const PolicyKey& key);

/*!
** \brief Read the content of a single entry as a bool
**
** If the local policies are not opened, the value is guaranted to be `defval`.
** \return True if the entry exists, false otherwise
*/
bool ReadAsBool(const PolicyKey& key, bool defval = false);

/*!
** \brief Dump the entries to the logs
*/
void DumpToLogs();

/*!
** \brief Dump to string
*/
void DumpToString(Yuni::Clob& out);

} // namespace LocalPolicy

#endif // __ANTARES_LIBS_ADMIN_POLICY_H__
