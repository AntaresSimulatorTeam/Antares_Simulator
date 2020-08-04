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
#ifndef __ANTARES_LIBS_SOLVER_H__
# define __ANTARES_LIBS_SOLVER_H__

# include <yuni/yuni.h>
# include "study/version.h"


namespace Antares
{
namespace Solver
{

	enum Feature
	{
		//! The standard solver
		standard = 0,
		//! With swap files
		withSwapFiles,
		//! The solver with years computed in parallel 
		parallel,
		//! Ortools solver
		ortools,
	};

	/*!
	** \brief Find the location of the solver
	** \return A non-empty string if the solver has been found, empty otherwise
	*/
	bool FindLocation(Yuni::String& location,
		Data::Version version = Data::versionUnknown, Feature features = standard);

	/*!
	** \brief Find the location of the analyzer
	** \return A non-empty string if the solver has been found, empty otherwise
	*/
	bool FindAnalyzerLocation(Yuni::String& location);

	/*!
	** \brief Find the location of the constraints builder
	** \return A non-empty string if the solver has been found, empty otherwise
	*/
	bool FindConstraintsBuilderLocation(Yuni::String& location);

	/*!
	** \brief Try to find the location of `antares-ybyaggregator`
	*/
	bool FindYearByYearAggregator(Yuni::String& filename);




} // namespace Solver
} // namespace Antares

#endif // __ANTARES_LIBS_SOLVER_H__
