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
#ifndef __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__
# define __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__

# include <antares/wx-wrapper.h>
# include <antares/study.h>
# include "../application/study.h"

# include <wx/sizer.h>
# include <wx/textctrl.h>
# include <wx/checkbox.h>
# include <wx/collpane.h>
# include <wx/dialog.h>
# include <wx/textctrl.h>
# include <wx/collpane.h>
# include <wx/valtext.h>



namespace Antares
{
namespace Window
{

	/*!
	** \brief `Save As` Dialog
	*/
	class SaveAs final : public wxDialog
	{
	public:
		/*!
		** \brief Execute the `SaveAs` dialog and save the given study accordingly
		**
		** \param parent The parent window
		** \param study The study
		** \return True if the study has been saved, false otherwise (cancelled for example)
		*/
		static SaveResult Execute(wxFrame* parent, Data::Study::Ptr study);

	public:
		enum
		{
			mnIDCollapsePanel = wxID_HIGHEST + 1,
			mnIDEditStudyName,
			mnIDEditStudyFolderName,
			mnIDBrowse,
		};

	public:
		// \name Constructors & Destructor
		//@{
		/*!
		** \brief Constructor with a parent window and a study
		*/
		explicit SaveAs(wxFrame* parent);
		/*!
		** \brief Constructor with a parent window and a study
		*/
		SaveAs(wxFrame* parent, Data::Study::Ptr study);
		//! Destructor
		virtual ~SaveAs();
		//@}

		//! \name Study
		//@{
		//! Get the attached study
		Data::Study::Ptr study();
		//! Get the attached study
		const Data::Study::Ptr study() const;
		//@}

		/*!
		** \brief Get if the study has been saved
		*/
		SaveResult result() const;


	private:
		//! Create components
		void internalCreateComponents();

		void onCollapsePanel(wxCollapsiblePaneEvent& evt);
		void onStudyNameChanged(wxCommandEvent& evt);
		void onStudyFolderNameChanged(wxCommandEvent& evt);
		void onBrowse(wxCommandEvent& evt);

		//! Event: The user has cancelled the operation
		void onCancel(void*);
		//! Event: Save the study !
		void onSave(void*);

		void propCaption(const wxString& text, bool bold = false);
		wxTextCtrl* propEdit(const wxString& defaultValue = wxString(), int id = wxID_ANY);
		wxWindow* propEdit(wxWindow* control);
		wxTextCtrl* propEdit(const wxString& defaultValue, const wxTextValidator& validator, int id = wxID_ANY);


	private:
		//! The study
		Data::Study::Ptr pStudy;
		//! Grid Sizer
		wxFlexGridSizer* pGridSizer;
		//!
		wxWindow* pParentProperties;
		//! Control for the name of the study
		wxTextCtrl* pStudyName;
		//! Control for the name of the study folder
		wxTextCtrl* pStudyFolderName;
		//! Control for the path
		wxTextCtrl* pFolder;
		//! True if the user set a custom study folder name
		bool pCustomFolderName;

		//! Checkbox for copying or not the output folder
		wxCheckBox* pCopyOutput;
		//! Checkbox for copying or not the user data folder
		wxCheckBox* pCopyUserData;
		//! Checkbox for copying or not the user data folder
		wxCheckBox* pCopyLogs;
		//! Result
		SaveResult pResult;
		// Event Table
		DECLARE_EVENT_TABLE()

	}; // class SaveAs






} // namespace Window
} // namespace Antares

# include "saveas.hxx"

#endif // __ANTARES_APPLICATION_WINDOWS_SAVEAS_H__
