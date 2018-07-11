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
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_EQUALS_H__
# define __ANTARES_TOOLBOX_FILTER_OPERATOR_EQUALS_H__

# include "../operator.h"


namespace Antares
{
namespace Toolbox
{
namespace Filter
{
namespace Operator
{

	class NotEqualsTo : public AOperator
	{
	public:
		NotEqualsTo(AFilterBase* parent)
			:AOperator(parent, wxT("!="), wxT("!="))
		{}
		virtual ~NotEqualsTo() {}

		virtual bool compute(const int a) const
		{
			return a != parameters[0].value.asInt;
		}

		virtual bool compute(const double a) const
		{
			return fabs(a - parameters[0].value.asDouble) > 0.00001;
		}

		virtual bool compute(const wxString& a) const
		{
			return a != parameters[0].value.asString;
		}

	}; // class NotEqualsTo



} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_EQUALS_H__
