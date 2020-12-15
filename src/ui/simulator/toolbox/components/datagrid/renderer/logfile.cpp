/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "logfile.h"
#include <yuni/core/math.h>

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
LogEntry::LogEntry() : line(0), highlight(false)
{
}

void LogEntry::assignVerbosity(const AnyString& pS)
{
    if (pS == "infos")
    {
        verbosity = "info";
        verbosityType = vtInfo;
        return;
    }
    if (pS == "notic")
    {
        verbosity = "notice";
        verbosityType = vtNotice;
        return;
    }
    if (pS == "check")
    {
        verbosity = "checkpoint";
        verbosityType = vtCheckpoint;
        return;
    }
    if (pS == "warns")
    {
        verbosity = "warning";
        verbosityType = vtWarning;
        return;
    }
    if (pS == "error")
    {
        verbosity = "error";
        verbosityType = vtError;
        return;
    }
    if (pS == "fatal")
    {
        verbosity = "fatal";
        verbosityType = vtError;
        return;
    }
    if (pS == "debug")
    {
        verbosity = "debug";
        verbosityType = vtDebug;
        return;
    }
    if (pS == "progress")
    {
        verbosity = "progress";
        verbosityType = vtInfo;
        return;
    }
}

LogEntryContainer::~LogEntryContainer()
{
    for (uint i = 0; i != entries.size(); ++i)
        delete entries[i];
}

LogFile::LogFile() : pControl(nullptr)
{
}

LogFile::~LogFile()
{
}

int LogFile::height() const
{
    return (!(!logs)) ? (int)logs->entries.size() : 0;
}

wxString LogFile::columnCaption(int x) const
{
    switch (x)
    {
    case 0:
        return wxT("                    Date                    ");
    // case 1: return wxT("Application");
    case 1:
        return wxT("Level");
    case 2:
        return wxT("                           Message                           ");
    }
    return wxEmptyString;
}

wxString LogFile::rowCaption(int rowIndx) const
{
    if (!(!logs) && static_cast<uint>(rowIndx) < logs->entries.size())
    {
        uint line = static_cast<uint>(logs->entries[rowIndx]->line);
        if (line)
            return wxString() << logs->entries[rowIndx]->line;
    }
    return wxEmptyString;
}

bool LogFile::cellValue(int, int, const Yuni::String&)
{
    return false;
}

double LogFile::cellNumericValue(int, int) const
{
    return 0.;
}

wxString LogFile::cellValue(int x, int y) const
{
    if (!logs || (uint)y >= logs->entries.size())
        return wxEmptyString;
    LogEntry& entry = *(logs->entries[y]);
    switch (x)
    {
    case 0:
        return wxStringFromUTF8(entry.date);
    // case 1: return wxStringFromUTF8(entry.application);
    case 1:
        return wxStringFromUTF8(entry.verbosity);
    case 2:
        return entry.message;
    }
    return wxEmptyString;
}

IRenderer::CellStyle LogFile::cellStyle(int, int) const
{
    return IRenderer::cellStyleCustom;
}

wxColour LogFile::cellBackgroundColor(int x, int y) const
{
    if (!logs || static_cast<uint>(y) >= logs->entries.size())
        return wxEmptyString;
    LogEntry& entry = *(logs->entries[y]);
    switch (entry.verbosityType)
    {
    case LogEntry::vtInfo:
        return entry.highlight ? wxColour(245, 247, 254) : wxColour(255, 255, 255);
    case LogEntry::vtWarning:
        return (x == 1) ? wxColour(255, 134, 12) : wxColour(255, 239, 222);
    case LogEntry::vtError:
        return (x == 1) ? wxColour(206, 0, 0) : wxColour(255, 228, 228);
    case LogEntry::vtCheckpoint:
        return (x == 1) ? wxColour(120, 136, 206) : wxColour(221, 228, 244);
    case LogEntry::vtNotice:
        return (x == 1) ? wxColour(50, 167, 34) : wxColour(236, 244, 233);
    case LogEntry::vtDebug:
        return wxColour(255, 255, 255);
    }
    return wxColour(255, 255, 255);
}

wxColour LogFile::cellTextColor(int x, int y) const
{
    if (!logs || static_cast<uint>(y) >= logs->entries.size())
        return wxEmptyString;
    LogEntry& entry = *(logs->entries[y]);
    switch (entry.verbosityType)
    {
    case LogEntry::vtInfo:
        return (x == 1) ? wxColour(121, 135, 163)
                        : (entry.highlight ? wxColour(95, 7, 7) : wxColour(42, 42, 47));
    case LogEntry::vtWarning:
        return (x == 1) ? wxColour(255, 255, 255) : wxColour(93, 47, 1);
    case LogEntry::vtError:
        return (x == 1) ? wxColour(255, 219, 219) : wxColour(66, 22, 22);
    case LogEntry::vtCheckpoint:
        return (x == 1) ? wxColour(255, 255, 255) : wxColour(61, 64, 84);
    case LogEntry::vtNotice:
        return (x == 1) ? wxColour(255, 255, 255) : wxColour(47, 70, 40);
    case LogEntry::vtDebug:
        return wxColour(177, 183, 192);
    }
    return wxColour();
}

wxColour LogFile::verticalBorderColor(int x, int y) const
{
    if (!logs || static_cast<uint>(y) >= logs->entries.size())
        return wxColour();
    LogEntry& entry = *(logs->entries[y]);
    if (!entry.line)
        return wxColour(240, 240, 240);
    if (x == 1 && entry.verbosityType == LogEntry::vtInfo)
        return wxColour(197, 202, 217);
    return IRenderer::verticalBorderColor(x, y);
}

int LogFile::cellAlignment(int x, int) const
{
    return (x == 2) ? -1 : 0;
}

int LogFile::columnWidthCustom(int x) const
{
    switch (x)
    {
    case 0:
    case 2:
        return -1;
    }
    return 0;
}

void LogFile::hintForColumnWidth(int x, wxString& out) const
{
    if (!x)
    {
        out = wxT(" Mon Apr  4 17:14:25 2011 ");
        return;
    }
    if (!logs || logs->longestLine >= logs->entries.size())
        return;
    out << logs->entries[logs->longestLine]->message;
}

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares
