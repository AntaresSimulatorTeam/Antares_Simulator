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
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__

#include <antares/wx-wrapper.h>
#include "../renderer.h"
#include "../../../../application/study.h"
#include <memory>
#include <vector>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
class LogEntry
{
public:
    //! Vector of log entries
    typedef std::vector<LogEntry*> Vector;
    //! Verbosity type
    enum VerbosityType
    {
        vtInfo,
        vtNotice,
        vtCheckpoint,
        vtWarning,
        vtError,
        vtDebug
    };

public:
    LogEntry();

    bool isWarningError() const
    {
        return (verbosityType == vtWarning || verbosityType == vtError);
    }

    void assignVerbosity(const AnyString& pS);

public:
    //! Type of the verbosity
    VerbosityType verbosityType;
    //! Line in the file
    uint line;
    // The most of the time, we will prefer customstring to consume less
    // memory for large logfiles
    //! Date
    Yuni::CString<32, false> date;
    //! Application
    Yuni::CString<32, false> application;
    //! Verbosity
    Yuni::CString<16, false> verbosity;
    //! The message itself
    wxString message;
    //! Highlight
    bool highlight;
};

class LogEntryContainer
{
public:
    //! The most suitable smart pointer for the class
    using Ptr = std::shared_ptr<LogEntryContainer>;

public:
    LogEntryContainer() : longestLine(0), maxCharForALine(0)
    {
    }
    ~LogEntryContainer();

public:
    //! Full filename
    YString filename;
    //! The bare filename
    YString barefilename;
    //! The total number of lines
    uint lines;
    //! Size
    Yuni::uint64 size;
    //! All entries
    LogEntry::Vector entries;
    //! The longest line
    uint longestLine;
    //! The maximum number of char found for a single line
    uint maxCharForALine;
};

class LogFile : public IRenderer
{
public:
    LogFile();
    virtual ~LogFile();

    virtual int width() const
    {
        return 3;
    }
    virtual int height() const;

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int x, int y) const;

    virtual double cellNumericValue(int x, int y) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    {
        // Do nothing
    }

    virtual bool valid() const
    {
        return !(!logs);
    }

    virtual uint maxWidthResize() const
    {
        return 0;
    }
    virtual IRenderer::CellStyle cellStyle(int col, int row) const;

    virtual wxColour cellBackgroundColor(int, int) const;
    virtual wxColour cellTextColor(int, int) const;

    virtual wxColour verticalBorderColor(int x, int y) const;

    void control(wxWindow* control)
    {
        pControl = control;
    }

    virtual int cellAlignment(int x, int y) const;

    virtual int columnWidthCustom(int x) const;

    virtual void hintForColumnWidth(int x, wxString& out) const;

public:
    //! Logs
    LogEntryContainer::Ptr logs;

protected:
    wxWindow* pControl;

}; // class LogFile

} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
