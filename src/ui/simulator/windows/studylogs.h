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
#ifndef __ANTARES_APPLICATION_WINDOWS_STUDY_LOGS_H__
# define __ANTARES_APPLICATION_WINDOWS_STUDY_LOGS_H__

# include <antares/wx-wrapper.h>
# include <antares/study.h>
# include <yuni/core/event.h>

# include <wx/htmllbox.h>
# include <wx/html/htmlwin.h>
# include <wx/stattext.h>
# include <wx/timer.h>
# include <wx/treectrl.h>

# include "../application/study.h"
# include "../toolbox/components/datagrid/renderer/logfile.h"
# include <ui/common/component/frame/local-frame.h>


namespace Antares
{
namespace Window
{


	/*!
	** \brief `Save As` Dialog
	*/
	class StudyLogs final : public Component::Frame::WxLocalFrame
	{
	public:
		typedef Component::Datagrid::Renderer::LogEntry           LogEntry;
		typedef Component::Datagrid::Renderer::LogEntryContainer  LogEntryContainer;

		class LogFilenameInfo final
		{
		public:
			//! Comparison
			struct ltstr final
			{
				bool operator()(const Yuni::String& s1, const Yuni::String& s2) const
				{
					return s2 < s1;
				}
			};

			//! Map
			typedef std::map<Yuni::String, LogFilenameInfo, ltstr> Map;

		public:
			//! The html display code
			wxString html;
			wxClientData* clientData;
		};

	public:
		// \name Constructors & Destructor
		//@{
		/*!
		** \brief Constructor with a parent window and a study
		*/
		StudyLogs(wxFrame* parent);
		//! Destructor
		virtual ~StudyLogs();
		//@}

		void refreshListOfAllAvailableLogs();

		void loadFromFile(const Yuni::String& filename);

		const Yuni::String& lastLogFile() const {return pLastLogFile;}

		void reloadFromData(LogEntryContainer::Ptr entries, LogEntryContainer::Ptr errors);

	private:
		/*!
		** \brief Refresh the file list
		*/
		void onButtonRefreshFileList(void*);
		/*!
		** \brief Reload the current opened log file
		*/
		void onReloadLogFile(void*);

		/*!
		** \brief The Window has been closed
		*/
		void onClose(wxCloseEvent& evt);

		/*!
		** \brief Updating the GUI to notify the user that a file is currently loading
		*/
		void enterLoadingMode(const Yuni::String& filename);

		void onChangeShowAllAdvanced(wxCommandEvent&);


	private:
		//! The last opened log file
		Yuni::String pLastLogFile;
		//! Button log filename
		Component::Button* pBtnLogFilenameRefresh;
		//! Label, from folder
		wxStaticText* pTxtFromFolder;
		//! The datagrid for displaying all log entries
		Component::Datagrid::Component* pLogDisplay;
		//! The datagrid for displaying all warnings/errors from the log file
		Component::Datagrid::Component* pLogDisplayErrors;
		//! The notebook
		Component::Notebook* pNotebook;

		//! Renderer
		Component::Datagrid::Renderer::LogFile* pRendererEntries;
		//! Renderer for errors and warnings
		Component::Datagrid::Renderer::LogFile* pRendererEntriesErrors;
		/*!
		** \brief The splitter window
		**
		** This component is used to separater the file list from the
		** display of a single log file.
		*/
		wxSplitterWindow* pSplitter;

		//! Allowed to close the window
		bool pCanCloseTheWindow;

		//! Temporary structure used for displaying the list of all available files
		LogFilenameInfo::Map pLogFilenameInfo;

		Yuni::Mutex pLoadingMutex;

		//! Flag to display all log files
		bool pShowAllLogsFiles;

		//! Event: refresh all files
		Yuni::Event<void (bool)> onRefreshListOfFiles;

		// Event Table
		DECLARE_EVENT_TABLE()

	}; // class StudyLogs






} // namespace Window
} // namespace Antares


#endif // __ANTARES_APPLICATION_WINDOWS_STUDY_LOGS_H__
