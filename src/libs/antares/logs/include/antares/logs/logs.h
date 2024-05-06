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

/*! Name of the app to use into logs */
#define LOG_APPLICATION_NAME "antares"
/*! Vendor */
#define LOG_APPLICATION_VENDOR "RTE"

/*! Special message to the interface */
#define LOG_UI "[UI] "

/*! Special message to the interface for notifying about a progression change */
#define LOG_PROGRESSION "[UI] progression: "

/*! Inform the GUI that the progression map is ready */
#define LOG_UI_PROGRESSION_MAP "[UI] Progression map: "

/*! Ask to the GUI to show the messages */
#define LOG_UI_DISPLAY_MESSAGES_ON "[UI] Display messages: On"
/*! Ask to the GUI to hide the messages */
#define LOG_UI_DISPLAY_MESSAGES_OFF "[UI] Display messages: Off"
/*! Ask to the GUI to hide the progress bar */
#define LOG_UI_HIDE_PROGRESSBAR "[UI] Progression: Off"
/*! Inform the GUI that the solver has properly ended */
#define LOG_UI_SOLVER_DONE "[UI] Quitting the solver gracefully"

#ifdef __cplusplus
// This section is only dedicated to includes from a c++ file
#include <yuni/core/logs.h>
#include <yuni/core/logs/decorators/applicationname.h>
#include <yuni/core/logs/handler/callback.h>

namespace Antares
{
namespace Data
{
// Forward declaration
class Study;

} // namespace Data
} // namespace Antares

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

#endif

#ifdef __cplusplus
extern "C"
{
#endif

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
void LogDisplayErrorInfos(uint errors, uint warnings, const char* message, bool printError = true);

#ifdef __cplusplus
}
#endif

#endif /* __ANTARES_LIBS_LOGS_LOGS_H__ */
