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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__
# define __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__


# include "scBuilderDataInterface.h"


namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{


	/*!
	** \brief Rules for hydro levels, for all years and areas
	*/
	class hydroLevelsData final : public dataInterface
	{
	public:
		//! Matrix
		typedef Matrix<double> MatrixType;

	public:

		// We use default constructor and destructor 

		//! \name Data manupulation
		//@{
		/*!
		** \brief Reset data from the study
		*/
		bool reset(const Study& study);

		/*!
		** \brief Export the data into a mere INI file
		*/
		void saveToINIFile(const Study& study, Yuni::IO::File::Stream& file) const;

		/*!
		** \brief Assign a single value
		**
		** \param index An area index
		** \param year  A year
		** \param value The new hydro level
		*/
		void set(uint index, uint year, String value);
		//@}

		uint width() const;

		uint height() const;

		double get_value(uint x, uint y) const;

		void set_value(uint x, uint y, String value);

		void apply(Study& study);

	private:
		//! Hydro levels overlay (0 if auto)
		MatrixType pHydroLevelsRules;

	}; // class hydroLevelsData

	// class hydroLevelsData : inline functions

	inline void hydroLevelsData::set(uint areaindex, uint year, String value)
	{
		assert(areaindex < pHydroLevelsRules.width);
		if (year < pHydroLevelsRules.height)
			pHydroLevelsRules[areaindex][year] = value.to<double>();
	}

	inline uint hydroLevelsData::width() const { return pHydroLevelsRules.width; }

	inline uint hydroLevelsData::height() const { return pHydroLevelsRules.height; }

	inline double hydroLevelsData::get_value(uint x, uint y) const { return pHydroLevelsRules.entry[y][x] * 100.; }


} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares


#endif // __LIBS_STUDY_SCENARIO_BUILDER_DATA_HYDRO_LEVELS_H__
