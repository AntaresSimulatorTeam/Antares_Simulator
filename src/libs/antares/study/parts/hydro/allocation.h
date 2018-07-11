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
#ifndef __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
# define __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__

# include "../../fwd.h"
# include <map>
# include <unordered_map>


namespace Antares
{
namespace Data
{

	class HydroAllocation final
	{
	public:
		//! Coefficients
		typedef std::map<AreaName, double>   Coefficients;

	public:
		//! \id Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		HydroAllocation();
		//! Destructor
		~HydroAllocation();
		//@}

		/*!
		** \brief Clear all coefficients
		*/
		void clear();

		/*!
		** \brief Remove coefficients related to a given area index
		*/
		void remove(const AreaName& areaid);

		void rename(const AreaName& oldid, const AreaName& newid);

		/*!
		** \brief Get the coefficient for a specific area
		*/
		double fromArea(const Area& area) const;
		/*!
		** \brief Get the coefficient for a specific area
		*/
		double fromArea(const Area* area) const;
		/*!
		** \brief Get the coefficient for a specific area index
		*/
		double fromArea(const AreaName& areaid) const;

		/*!
		** \brief Set the coefficient for a specific area (reference)
		*/
		void fromArea(const Area& area, double value);
		/*!
		** \brief Set the coefficient for a specific area (weak pointer)
		*/
		void fromArea(const Area* area, double value);
		/*!
		** \brief Set the coefficient for a specific area index
		*/
		void fromArea(const AreaName& areaid, double value);


		//! \name Load / Save
		//@{
		/*!
		** \brief Load allocation coefficients from a file
		*/
		bool loadFromFile(const AreaName& referencearea, const AnyString& filename);

		/*!
		** \brief Load allocation coefficients from a file
		*/
		bool saveToFile(const AnyString& filename) const;

		/*!
		** \brief Copy correlation coefficients from another correlation grid
		**
		** \param source The correlation matrix source
		** \param studySource The associated study for the given correlation grid
		** \param areaSource Name of the area to import
		** \param mapping Area's name Mapping from the source to the target
		** \param study Study for the local correlation grid
		*/
		void copyFrom(const HydroAllocation& source, const Study& studySource, const AreaName& areaSource,
			const AreaNameMapping& mapping, const Study& study);
		//@}


		//! \name Solver
		//@{
		/*!
		** \brief Iterate through all non-null coeff
		**
		** \code
		** area.hydroAllocation.eachNonNull([&] (uint areaindex, double value)
		** {
		** 	std::cout << "area: " << areaindex << " = " << value << std::endl;
		** });
		** \endcode
		**
		** \warning The internal map pValuesFromAreaID must be initialized
		**   prepareForSolver must be called.
		*/
		template<class CallbackT> void eachNonNull(const CallbackT& callback) const;

		/*!
		** \brief Prepare coefficients ordered by the area ids
		*/
		void prepareForSolver(const AreaList& list);
		//@}


		//! \name Operators
		//@{
		//! \see fromArea()
		double operator [] (const AreaName& areaid) const;
		//! \see fromArea()
		double operator [] (const Area& area) const;

		//! Get all coefficients
		const Coefficients& coefficients() const;
		//@}

	private:
		//! Coefficients for other areas
		Coefficients pValues;
		//! Coefficients for other areas, ordered by the area id
		std::unordered_map<uint, double>  pValuesFromAreaID;
		# ifndef NDEBUG
		//! Flag to prevent misuse of the bad container
		// TODO Find a more reliable way...
		bool pMustUseValuesFromAreaID;
		# endif

	}; // class HydroAllocation






} // namespace Data
} // namespace Antares

# include "allocation.hxx"

#endif // __ANTARES_LIB_STUDY_AREA_HYDRO_ALLOCATION_H__
