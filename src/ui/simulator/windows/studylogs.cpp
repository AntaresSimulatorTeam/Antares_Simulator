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

#include "studylogs.h"
#include <yuni/io/file.h>
#include <yuni/io/directory/info.h>
#include <yuni/job/job.h>

#include <wx/sizer.h>
#include <wx/dir.h>
#include <wx/regex.h>
#include <wx/clntdata.h>
#include <wx/wupdlock.h>
#include <wx/splitter.h>
#include <wx/imaglist.h>
#include <wx/checkbox.h>

#include "../application/main.h"
#include "../toolbox/resources.h"
#include "../toolbox/create.h"
#include "../toolbox/components/button.h"
#include "../toolbox/components/datagrid/component.h"
#include "../toolbox/components/datagrid/renderer/logfile.h"
#include <ui/common/component/panel.h>
#include <ui/common/lock.h>

#include <memory>

using namespace Yuni;

#define SEP Yuni::IO::Separator

namespace Antares
{
namespace Window
{
bool CompareDesc::operator()(const wxString& s1, const wxString& s2) const
{
    return s2 < s1;
}

BEGIN_EVENT_TABLE(StudyLogs, wxFrame)
EVT_CLOSE(StudyLogs::onClose)
END_EVENT_TABLE()

namespace // anonymous
{
class LogFile final : public Antares::Component::Spotlight::IItem
{
public:
    //! Ptr
    typedef std::shared_ptr<LogFile> Ptr;
    //! Vector of items
    typedef std::vector<Ptr> Vector;
    //! Vector Ptr
    typedef std::shared_ptr<Vector> VectorPtr;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    LogFile()
    {
    }
    //! Destructor
    virtual ~LogFile()
    {
    }
    //@}

public:
    String filename;

}; // class Text

inline void FindAllLogFiles(MapFileList& filelist, wxRegEx& regex, const AnyString& folder)
{
    filelist.clear();
    uint64 totalSize = 0u;

    if (not folder.empty())
    {
        wxString s;
        IO::Directory::Info info(folder);

        for (auto i = info.file_begin(); i != info.file_end(); ++i)
        {
            auto name = wxStringFromUTF8(*i);

            if (regex.Matches(name))
            {
                s.clear();
                // Date
                {
                    const wxString& pS = regex.GetMatch(name, 2);
                    s << pS[0] << pS[1] << pS[2] << pS[3] << wxT('/') << pS[4] << pS[5] << wxT('/')
                      << pS[6] << pS[7];
                }
                // Separator
                s << wxT(", ");
                // Hour
                {
                    const wxString& pS = regex.GetMatch(name, 3);
                    s << pS[0] << pS[1] << wxT(':') << pS[2] << pS[3] << wxT('.') << pS[4] << pS[5];
                }
                // Type
                {
                    const wxString& pS = regex.GetMatch(name, 1);
                    s << wxT(" - ") << pS;
                    s << wxT(" ") << (i.size() / 1024u) << wxT(" Ko");
                    totalSize += i.size();
                }
                // Keeping the filename somewhere
                filelist[s] = i.filename();
            }
        }
    }
}
} // anonymous namespace

FileListProvider::FileListProvider(StudyLogs& frame) :
 pFrame(frame), pAutoTriggerSelection(true), pShowAll(false)
{
    pBmpFile = std::shared_ptr<wxBitmap>(Resources::BitmapLoadFromFile("images/16x16/logs.png"));
}

FileListProvider::~FileListProvider()
{
    destroyBoundEvents(); // avoid corrupt vtable
}

void FileListProvider::search(Spotlight::IItem::Vector& out,
                              const Spotlight::SearchToken::Vector& tokens,
                              const Yuni::String& /* text */)
{
    bool hasAtLeastOneStudyLogEntry = false;
    {
        if (pAllSimuLogs.empty())
        {
            out.push_back(std::make_shared<Spotlight::Text>("  (no file available)"));
            hasAtLeastOneStudyLogEntry = true;
        }
        else
        {
            if (tokens.empty())
            {
                auto end = pAllSimuLogs.end();
                for (auto i = pAllSimuLogs.begin(); i != end; ++i)
                {
                    hasAtLeastOneStudyLogEntry = true;

                    auto item = std::make_shared<LogFile>();
                    item->caption(i->first);
                    item->filename = i->second;
                    item->image(pBmpFile.get());
                    if (pAutoTriggerSelection)
                    {
                        pAutoTriggerSelection = false;
                        item->select();
                        pFrame.loadFromFile(i->second);
                    }
                    out.push_back(item);
                }
            }
            else
            {
                String entry;
                Spotlight::SearchToken::Vector::const_iterator tend = tokens.end();
                auto end = pAllSimuLogs.end();
                for (auto i = pAllSimuLogs.begin(); i != end; ++i)
                {
                    wxStringToString(i->first, entry);
                    bool gotcha = false;
                    for (auto ti = tokens.begin(); ti != tend; ++ti)
                    {
                        const String& text = (*ti)->text;
                        if (entry.icontains(text))
                        {
                            gotcha = true;
                            break;
                        }
                    }

                    if (not gotcha)
                        continue;

                    hasAtLeastOneStudyLogEntry = true;
                    auto item = std::make_shared<LogFile>();
                    item->caption(i->first);
                    item->filename = i->second;
                    item->image(pBmpFile.get());
                    out.push_back(item);
                }
            }
        }
    }

    if (pShowAll)
    {
        if (hasAtLeastOneStudyLogEntry)
            out.push_back(std::make_shared<Spotlight::Text>());
        out.push_back(std::make_shared<Spotlight::Text>(" UI logs"));
        out.push_back(std::make_shared<Spotlight::Separator>());
        {
            if (pAllUILogs.empty())
            {
                out.push_back(std::make_shared<Spotlight::Text>("  (no file available)"));
            }
            else
            {
                if (tokens.empty())
                {
                    auto end = pAllUILogs.end();
                    for (auto i = pAllUILogs.begin(); i != end; ++i)
                    {
                        auto item = std::make_shared<LogFile>();
                        item->caption(i->first);
                        item->filename = i->second;
                        item->image(pBmpFile.get());
                        out.push_back(item);
                    }
                }
                else
                {
                    String entry;
                    uint count = 0;
                    auto tend = tokens.end();
                    auto end = pAllUILogs.end();
                    for (auto i = pAllUILogs.begin(); i != end; ++i)
                    {
                        wxStringToString(i->first, entry);
                        bool gotcha = false;
                        for (auto ti = tokens.begin(); ti != tend; ++ti)
                        {
                            const String& text = (*ti)->text;
                            if (entry.icontains(text))
                            {
                                gotcha = true;
                                break;
                            }
                        }
                        if (not gotcha)
                            continue;

                        ++count;
                        auto item = std::make_shared<LogFile>();
                        item->caption(i->first);
                        item->filename = i->second;
                        item->image(pBmpFile.get());
                        out.push_back(item);
                    }

                    if (0 == count)
                        out.push_back(std::make_shared<Spotlight::Text>("  (no result found)"));
                }
            }
        }
    }
}

bool FileListProvider::onSelect(Spotlight::IItem::Ptr& item)
{
    auto logfile = std::dynamic_pointer_cast<LogFile>(item);
    if (!logfile)
        return false;
    pFrame.loadFromFile(logfile->filename);
    return true;
}

bool FileListProvider::onSelect(const Spotlight::IItem::Vector&)
{
    return false;
}

void FileListProvider::refreshFileList(bool showAll)
{
    auto study = Data::Study::Current::Get();
    wxRegEx regex(wxT("([a-zA-Z_]+)-([0-9]{8})-([0-9]{6})\\.log"));
    String folder;
    pAllSimuLogs.clear();
    pAllUILogs.clear();

    // Simulation LOGS
    if (!(!study) and not study->folder.empty())
    {
        folder.clear() << study->folder << SEP << "logs";
        FindAllLogFiles(pAllSimuLogs, regex, folder);
    }

    // UI Simulator
    pShowAll = showAll;
    if (showAll)
    {
        folder.clear();
        IO::ExtractFilePath(folder, logs.logfile());
        FindAllLogFiles(pAllUILogs, regex, folder);
    }

    redoResearch();
}

class JobGUIUpdate final : public Yuni::Job::IJob
{
public:
    typedef Component::Datagrid::Renderer::LogEntry LogEntry;
    typedef Component::Datagrid::Renderer::LogEntryContainer LogEntryContainer;

public:
    explicit JobGUIUpdate(StudyLogs* window) : pWindow(window)
    {
    }

    virtual ~JobGUIUpdate()
    {
    }

public:
    LogEntryContainer::Ptr entries;
    LogEntryContainer::Ptr entriesErrors;

protected:
    virtual void onExecute() override
    {
        logs.debug() << "displaying logfile from GUI";
        pWindow->reloadFromData(entries, entriesErrors);
    }

private:
    StudyLogs* pWindow;
};

class JobLoadLogFile final : public Yuni::Job::IJob
{
public:
    typedef Component::Datagrid::Renderer::LogEntry LogEntry;
    typedef Component::Datagrid::Renderer::LogEntryContainer LogEntryContainer;
    typedef CString<1024 * 1024> BufferType;
    typedef Clob LineType;

public:
    explicit JobLoadLogFile(StudyLogs* window, const String& filename) :
     pStudyLogs(window), pEntries(nullptr), pEntriesErrors(nullptr)
    {
        IO::Normalize(pFilename, filename);
    }

    virtual ~JobLoadLogFile()
    {
    }

protected:
    virtual void onExecute() override
    {
        logs.info() << "log view " << pFilename;
        if (not IO::File::Exists(pFilename))
        {
            logs.error() << "File not found: " << pFilename;
            return;
        }
        const uint64 filesize = IO::File::Size(pFilename);

        bool shouldReopenLogFile = false;
        // Closing the current log file, to be able to open it
        if (logs.logfile() == pFilename)
        {
            shouldReopenLogFile = true;
            logs.closeLogfile();
        }

        // reset
        pEntries = std::make_shared<LogEntryContainer>();
        pEntriesErrors = std::make_shared<LogEntryContainer>();
        pLineIndex = 0u;

        // Read the file
        IO::File::Stream file(pFilename);
        if (not file.opened())
            logs.error() << "Impossible to open " << pFilename;
        else
        {
            // Read content from the file
            readLogFile(file);
            // Force closing the file, to be able to reopen it if required
            // (windows)
            file.close();
        }

        // reopening the log filename
        if (shouldReopenLogFile)
        {
            logs.reopenLogfile();
            logs.info() << "reopening " << logs.logfile();
        }

        pEntries->filename = pFilename;
        IO::ExtractFileName(pEntries->barefilename, pFilename);
        pEntries->lines = pLineIndex;
        pEntries->size = filesize;
        pEntriesErrors->lines = (uint)pEntriesErrors->entries.size();
        pEntriesErrors->size = 0;

        logs.info() << "  found " << pLineIndex << " lines, in " << filesize << " bytes";

        // Ask to update the GUI
        JobGUIUpdate* guiupdate = new JobGUIUpdate(pStudyLogs);
        guiupdate->entries = pEntries;
        guiupdate->entriesErrors = pEntriesErrors;

        // internal reset, just in case
        pEntries = nullptr;
        pEntriesErrors = nullptr;

        // GUI update
        Dispatcher::GUI::Post((const Yuni::Job::IJob::Ptr&)guiupdate, 80);
    }

    void readLogFile(IO::File::Stream& file)
    {
        // Most of the times, 1000 or even 2000 would be large enough.
        pEntries->entries.clear();
        pEntries->entries.reserve(2000);

        // reset
        pLastLineWasEmpty = true;
        pBuffer.resize(pBuffer.chunkSize);
        LineType line;
        AnyString adapter;

        while (file.read(pBuffer, pBuffer.chunkSize))
        {
            String::Size offset = 0;
            do
            {
                auto p = pBuffer.find('\n', offset);
                if (p == String::npos)
                {
                    // We only have a part of the line. We have to fetch the
                    // next piece of file for pursuing
                    if (offset < pBuffer.size())
                        line.append(pBuffer, pBuffer.size() - offset, offset);
                    break;
                }

                // New line !
                if (line.empty())
                {
                    // If the line is empty, we can directly use the buffer
                    // without copying it
                    // This can considerably speed up the whole process
                    if (offset < p)
                    {
                        adapter.adapt(pBuffer.c_str() + offset, p - offset);
                        if (!readLogLine(adapter))
                            addEmptyLine();
                    }
                    else
                        addEmptyLine();
                }
                else
                {
                    if (offset < p)
                        line.append(pBuffer, p - offset, offset);

                    if (!readLogLine(line))
                        addEmptyLine();
                    // reset
                    line.clear();
                }

                offset = p + 1;
            } while (true);
            pBuffer.resize(pBuffer.chunkSize);
        }

        // may happen if the file is not terminated by '\n'
        if (not line.empty())
        {
            if (!readLogLine(line))
                addEmptyLine();
        }
    }

    template<class StringT>
    bool readLogLine(StringT& line)
    {
        // Jumping to the next line
        ++pLineIndex;
        line.trimRight(" \t\r");

        // Example :
        // [Mon Apr  4 11:26:33 2011][solver][infos] My message here
        //
        if (line.size() < 38 /*arbitrary*/ or line.at(0) != '[')
            return false;
        if (line.at(25) != ']' or line.at(26) != '[')
            return false;

        const BufferType::Size applR = line.find(']', 27);
        if (BufferType::npos == applR)
            return false;
        const BufferType::Size verbosityR = line.find(']', applR + 1);
        if (BufferType::npos == verbosityR or applR >= verbosityR)
            return false;

        // alias to the raw data
        const char* const bcstr = line.c_str();

        verbosity.adapt(bcstr + applR + 2, verbosityR - applR - 2);
        if (verbosity == "progress") // should not create empty lines
            return true;
        if (verbosity == "progress" or line.size() <= verbosityR + 1)
            return false;

        // The message itself
        if (verbosityR - 2 >= line.size())
            return false;

        AnyString message;
        message.adapt(bcstr + verbosityR + 2, line.size() - verbosityR - 2);

        if (message.empty() or message.startsWith(LOG_UI))
            return false;

        date.adapt(bcstr + 1, 24);

        // Application
        if (applR > 27)
            application.adapt(bcstr + 27, applR - 27);
        else
            application.clear();

        LogEntry* entry = new LogEntry();
        pLastLineWasEmpty = false;

        entry->line = pLineIndex;
        // Date
        entry->date = date;
        // Application
        entry->application = application;
        // Verbosity level
        entry->assignVerbosity(verbosity);
        // Message
        entry->message = wxStringFromUTF8(message);
        // Because we don't have a proportional font...
        if (entry->verbosityType == LogEntry::vtInfo)
        {
            if (message.startsWith(' '))
            {
                if (message.startsWith("  :: "))
                {
                    entry->message.Replace(wxT("  :: "), wxT("    :: "));
                    entry->highlight = true;
                }
                else
                {
                    // silently ignore message about the local policy
                    // They are available anyway but they don't have to be visible
                    // from the interface.
                    if (message.startsWith("   added policy entry"))
                    {
                        delete entry;
                        return true;
                    }
                    entry->message.Replace(wxT("  "), wxT("    "));
                }
            }
            else
            {
                if (message.startsWith("Years from") or message.startsWith("Year ")
                    or message.startsWith("Summary"))
                    entry->highlight = true;
            }
        }

        // Adding the new entry
        pEntries->entries.push_back(entry);
        if (pEntries->maxCharForALine < message.size())
        {
            pEntries->maxCharForALine = message.size();
            pEntries->longestLine = (uint)pEntries->entries.size() - 1;
        }

        // We may want to reference this entry as an error
        if (entry->isWarningError())
        {
            LogEntry* copy = new LogEntry(*entry);
            pEntriesErrors->entries.push_back(copy);
            if (pEntriesErrors->maxCharForALine < message.size())
            {
                pEntriesErrors->maxCharForALine = message.size();
                pEntriesErrors->longestLine = (uint)pEntriesErrors->entries.size() - 1;
            }
        }

        // Ok, not considered as an empty line
        return true;
    }

    void addEmptyLine()
    {
        if (!pLastLineWasEmpty)
        {
            LogEntry* entry = new LogEntry();
            pEntries->entries.push_back(entry);
            pLastLineWasEmpty = true;
        }
    }

private:
    //! Reference to the window
    StudyLogs* pStudyLogs;
    //! The filename to read
    String pFilename;

    //! The date extracted from the log file
    YString date;
    //! The name of the application extracted from the log file
    YString application;
    //! The verbosity level extracted from the log file
    YString verbosity;
    //! All entries
    LogEntryContainer::Ptr pEntries;
    //! All entries, warning or error
    LogEntryContainer::Ptr pEntriesErrors;
    //! Current line within the file
    uint pLineIndex;
    //! Buffer for reading for the file
    BufferType pBuffer;
    //! Get if the last line was empty
    bool pLastLineWasEmpty;

}; // class JobLoadLogFile

StudyLogs::StudyLogs(wxFrame* parent) :
 Component::Frame::WxLocalFrame(
   parent,
   wxID_ANY,
   wxT("Logs"),
   wxDefaultPosition,
   wxSize(1000, 600),
   wxCAPTION | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN),
 pCanCloseTheWindow(true),
 pShowAllLogsFiles(false)
{
    pLastLogFile.reserve(FILENAME_MAX);

    wxIcon icon(Resources::WxFindFile("icons/study.ico"), wxBITMAP_TYPE_ICO);
    SetIcon(icon);

    // The main sizer
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    {
        pSplitter
          = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_NOBORDER);
        auto* panelAllFiles = new Component::Panel(pSplitter);
        wxSizer* sizerAllFiles = new wxBoxSizer(wxVERTICAL);
        panelAllFiles->SetSizer(sizerAllFiles);

        // Refresh all available files
        {
            wxSizer* hzAllFiles = new wxBoxSizer(wxHORIZONTAL);
            hzAllFiles->AddSpacer(8);
            Component::Button* btnRefresh
              = new Component::Button(panelAllFiles,
                                      wxT("Refresh the file list"),
                                      "images/16x16/refresh.png",
                                      this,
                                      &StudyLogs::onButtonRefreshFileList);
            hzAllFiles->Add(btnRefresh, 0, wxALL | wxEXPAND);
            sizerAllFiles->AddSpacer(5);
            sizerAllFiles->Add(hzAllFiles, 0, wxALL | wxEXPAND, 2);
            sizerAllFiles->AddSpacer(4);
        }
        // List of all available log files
        {
            Component::Spotlight* spotlight = new Component::Spotlight(panelAllFiles, 0);
            mProvider = std::make_shared<FileListProvider>(*this);
            onRefreshListOfFiles.connect(mProvider.get(), &FileListProvider::refreshFileList);
            spotlight->provider(mProvider);

            {
                wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
                hz->AddSpacer(10);
                hz->Add(spotlight, 1, wxALL | wxEXPAND);
                hz->SetItemMinSize(spotlight, 240, 100);
                sizerAllFiles->Add(hz, 1, wxALL | wxEXPAND);
            }
            // Advanced settings
            {
                wxBoxSizer* hz = new wxBoxSizer(wxHORIZONTAL);
                wxCheckBox* allLogFiles = new wxCheckBox(panelAllFiles,
                                                         wxID_ANY,
                                                         wxT(" Show all log info  (advanced) "),
                                                         wxDefaultPosition,
                                                         wxDefaultSize);
                hz->AddSpacer(15);
                hz->Add(allLogFiles, 0, wxALL | wxEXPAND);
                hz->AddSpacer(15);
                sizerAllFiles->AddSpacer(8);
                sizerAllFiles->Add(hz, 0, wxALL | wxEXPAND);
                sizerAllFiles->AddSpacer(8);

                allLogFiles->Connect(allLogFiles->GetId(),
                                     wxEVT_COMMAND_CHECKBOX_CLICKED,
                                     wxCommandEventHandler(StudyLogs::onChangeShowAllAdvanced),
                                     nullptr,
                                     this);
            }
        }

        auto* logfilepanel = new Component::Panel(pSplitter);
        auto* logfilesizer = new wxBoxSizer(wxVERTICAL);
        logfilepanel->SetSizer(logfilesizer);

        auto* notebook = new Component::Notebook(logfilepanel, Component::Notebook::orTop);
        notebook->theme(Component::Notebook::themeLight);
        pNotebook = notebook;

        auto* infopanel = new Component::Panel(logfilepanel);
        logfilesizer->Add(infopanel, 0, wxALL | wxEXPAND);
        logfilesizer->Add(notebook, 1, wxALL | wxEXPAND);
        auto* infosizer = new wxBoxSizer(wxVERTICAL);
        auto* infohz = new wxBoxSizer(wxHORIZONTAL);
        infohz->AddSpacer(7);
        infohz->Add(infosizer, 1, wxALL | wxEXPAND, 2);

        pBtnLogFilenameRefresh = new Component::Button(infopanel,
                                                       wxEmptyString,
                                                       "images/16x16/refresh_table.png",
                                                       this,
                                                       &StudyLogs::onReloadLogFile);
        pBtnLogFilenameRefresh->bold(true);
        pBtnLogFilenameRefresh->caption(wxT("Please select a log file"));
        infosizer->AddSpacer(5);
        infosizer->Add(pBtnLogFilenameRefresh, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        infosizer->AddSpacer(1);

        auto* pathsizer = new wxBoxSizer(wxHORIZONTAL);
        pathsizer->Add(23, 5);
        pTxtFromFolder = Component::CreateLabel(infopanel, wxEmptyString, false, false, -1);
        pathsizer->Add(pTxtFromFolder, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        infosizer->Add(pathsizer, 0, wxALL | wxEXPAND);
        infosizer->AddSpacer(22);
        infopanel->SetSizer(infohz);

        // Split the view
        if (System::windows)
            pSplitter->SetMinimumPaneSize(250);
        else
            pSplitter->SetMinimumPaneSize(300);

        pSplitter->SplitVertically(panelAllFiles, logfilepanel);
        // pSplitter->SetSashGravity(0.3);
        pSplitter->SetSashPosition(350, true);
        sizer->Add(pSplitter, 1, wxALL | wxEXPAND);
        sizer->SetItemMinSize(pSplitter, 200, 200);

        // Grids
        pRendererEntries = new Component::Datagrid::Renderer::LogFile();
        pLogDisplay = new Component::Datagrid::Component(
          notebook, pRendererEntries, wxEmptyString, false, true, true, true);
        pRendererEntries->control(pLogDisplay);
        Component::Notebook::Page* page;

        page = notebook->add(pLogDisplay, wxT("all"), wxT(" All "));
        page->visible(false);

        pRendererEntriesErrors = new Component::Datagrid::Renderer::LogFile();
        pLogDisplayErrors = new Component::Datagrid::Component(
          notebook, pRendererEntriesErrors, wxEmptyString, false, true, true, true);
        pRendererEntriesErrors->control(pLogDisplayErrors);
        page = notebook->add(pLogDisplayErrors, wxT("errors"), wxT("Warnings & Errors"));
        page->visible(false);

        notebook->select(wxT("all"));
    }

    // Content
    SetSizer(sizer);
    sizer->Layout();

    refreshListOfAllAvailableLogs();
}

StudyLogs::~StudyLogs()
{
    auto* wnd = Forms::ApplWnd::Instance();
    if (wnd)
        wnd->pWndLogs = nullptr;

    pLoadingMutex.lock();
    pLogFilenameInfo.clear();
    pLoadingMutex.unlock();
}

void StudyLogs::onButtonRefreshFileList(void*)
{
    refreshListOfAllAvailableLogs();
}

void StudyLogs::refreshListOfAllAvailableLogs()
{
    onRefreshListOfFiles(pShowAllLogsFiles);
}

void StudyLogs::loadFromFile(const Yuni::String& filename)
{
    enterLoadingMode(filename);
    auto* job = new JobLoadLogFile(this, filename);
    Dispatcher::Post((const Yuni::Job::IJob::Ptr&)job);
    // Avoid undefined behavior
    if (&filename != &pLastLogFile)
        pLastLogFile = filename;
}

void StudyLogs::onClose(wxCloseEvent& evt)
{
    if (not pCanCloseTheWindow)
    {
        evt.Veto(true);
    }
    else
    {
        Forms::ApplWnd::Instance()->pWndLogs = nullptr;
        Destroy();
    }
}

void StudyLogs::enterLoadingMode(const String& filename)
{
    // We can no longer close the window
    pCanCloseTheWindow = false;

    String name;
    String path;
    IO::ExtractFileName(name, filename);
    IO::ExtractFilePath(path, filename);

    // Updating the notebook
    pNotebook->caption(wxEmptyString);
    auto* page = pNotebook->find(wxT("errors"));
    if (page)
        page->caption(wxString(wxT("Warnings & Errors")));

    // Window title
    SetTitle(wxString(wxT("Logs:  Loading  ")) << wxStringFromUTF8(name));

    // Refresh button
    pBtnLogFilenameRefresh->caption(wxString(wxT("Loading  ")) << wxStringFromUTF8(name));
    // Path
    pTxtFromFolder->SetLabel(wxString() << wxT("from  ") << wxStringFromUTF8(path));

    // Renderers
    pRendererEntries->logs = nullptr;
    pRendererEntriesErrors->logs = nullptr;
    // Containers
    pLogDisplay->forceRefresh();
    pLogDisplayErrors->forceRefresh();

    // Refresh of the whole window
    Refresh();
}

void StudyLogs::reloadFromData(LogEntryContainer::Ptr entries, LogEntryContainer::Ptr errors)
{
    if (!entries or not errors)
    {
        assert(false and "studylogs: reloadFromData: invalid parameters");
        return;
    }
    assert(pNotebook);

    wxString title;
    title << wxT("Logs:  ") << wxStringFromUTF8(entries->barefilename);
    SetTitle(title);

    pBtnLogFilenameRefresh->caption(wxString(wxT("Reload  "))
                                    << wxStringFromUTF8(entries->barefilename));
    pRendererEntries->logs = entries;
    pRendererEntriesErrors->logs = errors;
    pLogDisplay->forceRefresh();
    pLogDisplayErrors->forceRefresh();

    const bool noErrorsEntry = errors->entries.empty();

    auto* page = pNotebook->find(wxT("errors"));
    if (page)
    {
        if (noErrorsEntry)
        {
            page->visible(false);
            pNotebook->select(wxT("all"));
        }
        else
        {
            page->visible(true);
            page->caption(wxString(wxT(" Warnings & Errors  (")) << errors->lines << wxT(") "));
        }
    }

    page = pNotebook->find(wxT("all"));
    if (page)
    {
        page->caption(wxString(wxT(" All  (")) << entries->lines << wxT(") "));
        page->visible(true);
    }

    Dispatcher::GUI::Refresh(this);
    Dispatcher::GUI::Refresh(pNotebook);

    // We can longer close the window now
    pCanCloseTheWindow = true;
}

void StudyLogs::onReloadLogFile(void*)
{
    if (not pLastLogFile.empty() and pCanCloseTheWindow)
        loadFromFile(pLastLogFile);
}

void StudyLogs::onChangeShowAllAdvanced(wxCommandEvent& evt)
{
    pShowAllLogsFiles = evt.IsChecked();
    refreshListOfAllAvailableLogs();
}

} // namespace Window
} // namespace Antares
