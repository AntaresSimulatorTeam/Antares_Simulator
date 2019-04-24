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
#ifndef __SOLVER_VARIABLE_SURVEY_RESULTS_SURVEY_RESULTS_H__
# define __SOLVER_VARIABLE_SURVEY_RESULTS_SURVEY_RESULTS_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include "../constants.h"
# include <antares/study.h>
# include "../categories.h"
# include <antares/timeelapsed/timeelapsed.h>
# include "data.h"
# include <antares/study/variable-print-info.h>



namespace Antares
{
namespace Solver
{
namespace Variable
{


	/*!
	** \brief Class utility for building CSV results files
	*/
	class SurveyResults
	{
	public:
		//! Precision
		typedef Yuni::CString<10,false>  PrecisionType;
		//! Caption
		typedef Yuni::CString<128,false> CaptionType;

		/*!
		** \brief Try to estimate theamount of memory required by the class
		*/
		static void EstimateMemoryUsage(uint maxVars, Data::StudyMemoryUsage& u);

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		**
		** \param maxVars The maximum total of variables that we may need for writing
		**   the report
		** \param s Reference to the study
		** \param o The output folder
		** \param year The current year, if any
		*/
		SurveyResults(uint maxVars, const Data::Study& s, const Yuni::String& o, uint year = (uint)-1);
		/*!
		** \brief Destructor
		*/
		~SurveyResults();
		//@}


		/*!
		** \brief Write the data into a file
		*/
		void saveToFile(int dataLevel, int fileLevel, int precisionLevel);

		/*!
		** \brief Export informations about the current study
		**
		** It is composed by several files to completely describe the system
		** and provide a good support for Excel macros.
		*/
		void exportGridInfos();

		bool createDigestFile();

		// Reset a line of values to zero.
		void resetValuesAtLine(uint);

		/*!
		** \brief Export the digest file
		*/
		void exportDigestAllYears();

		void exportDigestMatrix(const char* title);

	public:
		//! Data (not related to the template parameter)
		Solver::Variable::Private::SurveyResultsData data;

		//! Caption for the current variable
		Yuni::CString<128,false> variableCaption;

		//! Matrix where to store all results
		double** values;

		enum
		{
			captionCount = 3,
		};
		//! Array to store all variable names
		CaptionType* captions[captionCount];

		//! Precision to for each column (in the printf format)
		PrecisionType* precision;

		//! Non applicable status for each column (in the printf format)
		bool* nonApplicableStatus;
		// Digest file non applicable status ( dim : nb vars x max(nb areas, nb sets of areas) )
		uint digestSize; // Useful dimension for digest file non applicable statut management. 
		bool** digestNonApplicableStatus;

		//! The total number of variables
		const uint maxVariables;

		//! Flag to known if we are in the year-by-year mode
		bool yearByYearResults;

		//! When looping over output variables, is current variable non applicable ?
		//! In the static type list of variables, there is a need to convey the non applicable status to variables
		//! statistic results through an instance of the current class.
		//! Furthermore, some unusual variables are actually "multi-variables", that is they contain actually
		//! several variables.
		//! Therefore, the following is a pointer on the current ouput variable's non applicable status,
		//! and in case of a multi-variable, on the current sub-variable's non applicable status.
		bool* isCurrentVarNA;
		//! Same thing for print status (do we print the current output variable ?)
		bool* isPrinted;

	private:
		/*!
		** \brief Export informations about each area (and its sub-components - thermal clusters, links)
		*/
		void exportGridInfosAreas(const Yuni::String& folder);

		template<class StringT, class ConvertT, class PrecisionT>
		void AppendDoubleValue(uint& error, const double v, StringT& buffer, ConvertT& conversionBuffer, const PrecisionT& precision, const bool isNotApplicable);

		void writeDateToFileDescriptor(uint row, int fileLevel, int precisionLevel);

	}; // class SurveyResults






} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_SURVEY_RESULTS_SURVEY_RESULTS_H__
