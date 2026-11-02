// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_HOST_INFO_H__
#define __ANTARES_LIBS_HOST_INFO_H__

#include <string>

namespace Antares::Logs
{
/*!
** \brief Canonical name of the machine, "<unknown>" if it cannot be resolved
*/
std::string hostName();
} // namespace Antares::Logs

/*!
** \brief Write the host info into logs
*/
void WriteHostInfoIntoLogs();

#endif // __ANTARES_LIBS_HOST_INFO_H__
