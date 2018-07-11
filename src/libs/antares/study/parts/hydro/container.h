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
#ifndef __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__
# define __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__

# include "prepro.h"
# include "series.h"
# include "../../fwd.h"
# include "allocation.h"



namespace Antares
{
namespace Data
{

	/*!
	** \brief Hydro for a single area
	*/
	class PartHydro
	{
	public:
		enum
		{
			//! The minimum value
			minimum = 0,
			//! The average value
			average,
			//! The maximum value
			maximum,
		};

	public:
		/*!
		** \brief Load data for hydro container from a folder
		**
		** \param folder The targer folder
		** \return A non-zero value if the operation succeeded, 0 otherwise
		*/
		static bool LoadFromFolder(Study& study, const AnyString& folder);

		/*!
		** \brief Save data from several containers to a folder (except data for the prepro and time-series)
		**
		** \param l List of areas
		** \param folder The targer folder
		** \return A non-zero value if the operation succeeded, 0 otherwise
		*/
		static bool SaveToFolder(const AreaList& areas, const AnyString& folder);


	public:
		/*!
		** \brief Default Constructor
		*/
		PartHydro();
		//! Destructor
		~PartHydro();

		/*!
		** \brief Reset internal data
		*/
		void reset();

		void copyFrom(const PartHydro& rhs);

		bool invalidate(bool reload = false) const;

		/*!
		** \brief Mark all data as modified
		*/
		void markAsModified() const;

	public:
		//! Inter-daily breakdown (previously called Smoothing Factor or alpha)
		double interDailyBreakdown;
		//! Intra-daily modulation
		double intraDailyModulation;

		//! Intermonthly breakdown
		double intermonthlyBreakdown;

		//! Enabled reservoir management
		bool reservoirManagement;
		//! Reservoir capacity (MWh)
		double reservoirCapacity;

		//! Daily max power ({min,avg,max}x365)
		Matrix<double, uint> maxPower;

		// Useful for solver RAM estimation
		bool hydroModulable;

		//! Monthly reservoir level ({min,avg,max}x12)
		Matrix<double> reservoirLevel;

		//! Hydro allocation, from other areas
		HydroAllocation allocation;

		//! Data for the pre-processor
		PreproHydro* prepro;
		//! Data for time-series
		DataSeriesHydro* series;

	}; // class PartHydro







} // namespace Data
} // namespace Antares

# include "../../area.h"

#endif /* __ANTARES_LIBS_STUDY_PARTS_HYDRO_CONTAINER_H__ */
