// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__

#include "../renderer.h"
#include "../../../../application/study.h"
#include <memory>
#include <vector>

namespace Antares::Component::Datagrid::Renderer
{
class LogEntry
{
public:
    //! Vector of log entries
    using Vector = std::vector<LogEntry*>;

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
    LogEntryContainer():
        longestLine(0),
        maxCharForALine(0)
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
    uint64_t size;
    //! All entries
    LogEntry::Vector entries;
    //! The longest line
    uint longestLine;
    //! The maximum number of char found for a single line
    uint maxCharForALine;
};

class LogFile: public IRenderer
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

} // namespace Antares::Component::Datagrid::Renderer

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_MC_PLAYLIST_H__
