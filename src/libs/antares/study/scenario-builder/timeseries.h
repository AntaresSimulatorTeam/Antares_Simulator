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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_TIMESERIES_H__
# define __LIBS_STUDY_SCENARIO_BUILDER_TIMESERIES_H__

# include <yuni/yuni.h>
# include "../../array/matrix.h"
# include "../fwd.h"
# include <yuni/core/noncopyable.h>


namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{


	/*!
	** \brief Rules for TS numbers, for all years and a single timeseries
	*/
	class TSNumberRules final : private Yuni::NonCopyable<TSNumberRules>
	{
	public:
		//! Matrix
		typedef Matrix<Yuni::uint32> MatrixType;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor (thermal by default)
		*/
		TSNumberRules();
		/*!
		** \brief Default constructor
		**
		** \param tstype Type of the timeseries
		*/
		TSNumberRules(TimeSeries tstype);
		//! Destructor
		~TSNumberRules();
		//@}

		//! \name Data manupulation
		//@{
		/*!
		** \brief Reset data from the study
		*/
		bool reset(const Study& study);

		/*!
		** \brief Load data from the study
		*/
		bool loadFromStudy(const Study& study);

		/*!
		** \brief Export the data into a mere INI file
		*/
		void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const;

		/*!
		** \brief Assign a single value
		**
		** \param index An area index or a thermal cluster index
		** \param year  A year
		** \param value The new TS number
		*/
		void set(uint index, uint year, uint value);
		//@}

		//! \name Rules matrix
		//@{
		/*!
		** \brief Resize to a given number of years
		*/
		void resize(uint nbYears);

		//! Get the overlay matrix
		const MatrixType& overlay() const;
		//! Get the overlay matrix
		MatrixType& overlay();
		//@}

		//! \name Area
		//@{
		//! Attach an area
		void attachArea(const Area* area);
		//! Get the attached area, if any
		const Area*  area() const;
		//@}

		/*!
		** \brief Apply the changes to the timeseries number matrices
		**
		** This method is only useful when launched from the solver.
		*/
		void apply(const Study& study);

	private:
		//! Type of the timeseries
		const TimeSeries pTSType;
		//! All TS number overlay (0 if auto)
		MatrixType pTSNumberRules;
		//! The attached area, if any
		const Area* pArea;

	}; // class TSNumberRules






} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

# include "timeseries.hxx"

#endif // __LIBS_STUDY_SCENARIO_BUILDER_TIMESERIES_H__
