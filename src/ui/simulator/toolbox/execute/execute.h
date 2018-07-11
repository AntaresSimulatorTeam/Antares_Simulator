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
#ifndef __ANTARES_TOOLBOX_E_EXECUTE_H__
# define __ANTARES_TOOLBOX_E_EXECUTE_H__

# include <antares/wx-wrapper.h>


namespace Antares
{
namespace Toolbox
{
namespace Process
{



	class Execute
	{
	public:
		//@{
		/*!
		** \brief Default constructor
		*/
		Execute();
		/*!
		** \brief Destructor
		*/
		~Execute();
		//@}


		//! \name Command to execute
		//@{
		//! Get the command
		const wxString& command() const {return pCommand;}
		//! Set the command to execute
		void command(const wxString& v) {pCommand = v;}
		//@}

		//! \name Working directory
		//@{
		//! Get the working directory to use before executing the command
		const wxString& workingDirectory() const {return pWDirectory;}
		//! Set the working directory
		void workingDirectory(const wxString& v) {pWDirectory = v;}
		//@}


		//! \name Title
		//@{
		//! Get the sub-title value
		const wxString& title() const {return pTitle;}
		//! Set the title
		void title(const wxString& v) {pTitle = v;}
		//@}

		//! \name Title
		//@{
		//! Get the sub-title value
		const wxString& subTitle() const {return pSubTitle;}
		//! Set the sub-title
		void subTitle(const wxString& v) {pSubTitle = v;}
		//@}

		//! \name The progress bar
		//@{
		//! Get if the progress bar can be displayed
		bool displayProgressBar() const {return pDisplayProgressBar;}
		//! Set if the progress bar can be displayed
		void displayProgressBar(const bool v) {pDisplayProgressBar = v;}
		//@}

		//! \name Icon
		//@{
		//! Get the relative address to the icon in the resources folder
		const char* icon() const {return pIcon;}
		//! Set the relative address to the icon in the resources folder
		void icon(const char* v) {pIcon = v;}
		//@}

		//! \name Execution workflow
		//@{
		/*!
		** \brief Run the command
		*/
		bool run();
		//@}

	private:
		//! The title
		wxString pTitle;
		//! The sub-title
		wxString pSubTitle;
		//! The command to execute
		wxString pCommand;
		//! The working directory
		wxString pWDirectory;
		//!
		bool pDisplayProgressBar;
		//! The icon
		const char* pIcon;

	}; // class Execute





} // namespace Process
} // namespace Toolbox
} // namespace Antares


#endif // __ANTARES_TOOLBOX_E_EXECUTE_H__
