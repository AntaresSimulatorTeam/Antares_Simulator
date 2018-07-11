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
#ifndef __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__
# define __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__

# include <vector>
# include <antares/wx-wrapper.h>



namespace Antares
{
namespace Toolbox
{
namespace Filter
{

	// Forward declaration
	class AFilterBase;

namespace Operator
{

	// Forward declaration
	class AOperator;




	/*!
	** \brief Operator list
	*/
	class List
	{
	public:
		//! \name Constructor && Destructor
		//@{
		//! Constructor
		List(AFilterBase* parent);
		//! Destructor
		~List();
		//@}

		/*!
		** \brief Clear the container
		*/
		void clear();

		/*!
		** \brief Add an operator from its given name
		**
		** \param name Name of the operator
		** \return True if an operator has been added, false if not found
		*/
		bool add(const wxString& name);

		void addStdArithmetic(bool withModulo = true);
		void addStdWeekday(bool withModulo = false);
		void addStdMonth(bool withModulo = false);

		uint size() const {return (uint) pItems.size();}
		uint count() const {return (uint) pItems.size();}

		//! Get if the list is empty
		bool empty() const {return pItems.empty();}

		//! Get the parent filter
		AFilterBase* parentFilter() const {return pParentFilter;}

		/*!
		** \brief Get the operator at a given index
		*/
		AOperator* at(const int indx) const throw() {return pItems[indx];}

		//! Operator []
		AOperator* operator [] (const int indx) const throw() {return pItems[indx];}


	private:
		bool internalAdd(const wxString& name);

	private:
		//! Parent filter
		AFilterBase* pParentFilter;
		//! Operator list (typedef)
		typedef std::vector<AOperator*> OperatorList;
		//! Operator list
		OperatorList pItems;

	}; // class List




} // namespace Operator
} // namespace Filter
} // namespace Toolbox
} // namespace Antares

# include "operator.h"

#endif // __ANTARES_TOOLBOX_FILTER_OPERATOR_LIST_H__
