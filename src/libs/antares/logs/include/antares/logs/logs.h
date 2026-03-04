// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_LOGS_LOGS_H__
#define __ANTARES_LIBS_LOGS_LOGS_H__

#include <yuni/yuni.h>
#include "yuni/core/logs/decorators/message.h"
#include "yuni/core/logs/decorators/time.h"
#include "yuni/core/logs/decorators/verbositylevel.h"
#include "yuni/core/logs/handler/file.h"
#include "yuni/core/logs/handler/stdcout.h"
#include "yuni/core/logs/logs.h"

/*!
** \defgroup logs Logs
** \ingroup toolbox
*/

// This section is only dedicated to includes from a c++ file
#include <yuni/core/logs.h>
#include <yuni/core/logs/decorators/applicationname.h>
#include <yuni/core/logs/handler/callback.h>

namespace Antares
{
//! Handlers for logging
using LoggingHandlers = Yuni::Logs::StdCout< // For writing to the standard output
  Yuni::Logs::File<                          // For writing into a log file
    Yuni::Logs::Callback<>                   // Callback
    >>;

//! Decorators for logging
using LoggingDecorators = Yuni::Logs::Time< // Date/Time when the entry log is added
  Yuni::Logs::ApplicationName<              // Name of the current running application
    Yuni::Logs::VerbosityLevel<             // Verbosity level (info, warning...)
      Yuni::Logs::Message<>                 // The real message
      >>>;

//! Our log facility
extern Yuni::Logs::Logger<LoggingHandlers, LoggingDecorators> logs;

} // namespace Antares

/*!
** \brief Levels for logging
*/
enum LogLevel
{
    logUnknown = 0,
    logProgress, /* not really a real log level */
    logFatal,
    logError,
    logWarning,
    logNotice,
    logInfo,
    logDebug
};

/*!
** \brief Generate a compatibility notice
** \ingroup logs
**
** \param format The format string
** \return Always 0
*/
int LogCompatibility(const char format[], ...);

/*!
** \brief Display informations about encountered errors
*/
void LogDisplayErrorInfos(uint errors,
                          uint warnings,
                          const char* message,
                          bool printAsError = true);

#endif /* __ANTARES_LIBS_LOGS_LOGS_H__ */
