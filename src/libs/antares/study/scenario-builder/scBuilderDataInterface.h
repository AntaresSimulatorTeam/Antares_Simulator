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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__
# define __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__

#include "../study.h"
# include <yuni/core/noncopyable.h>

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{


	/*!
	** \brief Interface for scenario builder data (time series, hydro levels, ...)
	*/
	class dataInterface : private Yuni::NonCopyable<dataInterface>
	{
	public:
		//! \name Data manupulation
		//@{
		/*!
		** \brief Reset data from the study
		*/
		virtual
		bool reset(const Study& study) = 0;

		virtual
		void set(uint index, uint year, String value) = 0;

		/*!
		** \brief Export the data into a mere INI file
		*/
		virtual
		void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const = 0;

		virtual
		uint width() const = 0;

		virtual
		uint height() const = 0;

		virtual
		double get_value(uint x, uint y) const = 0;

		virtual
		void set_value(uint x, uint y, String value) = 0;

		/*!
		** \brief Apply the changes to the study corresponding data (time series, hydro levels, ...)
		**
		** This method is only useful when launched from the solver.
		*/
		virtual
		void apply(Study& study) = 0;

	}; // class dataInterface






} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_INTERFACE_H__
