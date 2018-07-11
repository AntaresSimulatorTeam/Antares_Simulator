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
#ifndef __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
# define __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__


namespace Antares
{
namespace Action
{
namespace AntaresStudy
{
namespace Constraint
{


	template<class StringT>
	inline Create::Create(const StringT& areaname, Data::BindingConstraint::Type t, Data::BindingConstraint::Operator o)
		:pOriginalConstraintName(areaname),
		pType(t), pOperator(o)
	{
		pInfos.behavior = bhOverwrite;
	}


	template<class StringT1, class StringT2>
	inline Create::Create(const StringT1& areaname, const StringT2& targetname, Data::BindingConstraint::Type t, Data::BindingConstraint::Operator o)
		:pOriginalConstraintName(areaname), pTargetConstraintName(targetname),
		pType(t), pOperator(o)
	{
		pInfos.behavior = bhOverwrite;
	}


	inline Create::~Create()
	{}


	inline bool Create::allowUpdate() const
	{
		return true;
	}





} // namespace Constraint
} // namespace AntaresStudy
} // namespace Action
} // namespace Antares

#endif // __ANTARES_LIBS_STUDY_ACTION_HANDLER_ANTARES_CONSTRAINT_CREATE_HXX__
