/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once



namespace Yuni
{

	/*!
	** \brief Folding implementation
	** \ingroup Functional
	**
	** \code
	** bool gotSomething = fold(data, false, [] (bool& result, const A& current) -> bool
	** {
	**	if (current.isModified())
	**	{
	**		result = true;
	**		current.doSomething();
	**	}
	**	return true;
	** });
	** \endcode
	*/
	template<class ResultT, class ContainerT, class AccumulatorT>
	YUNI_DECL ResultT fold(const ContainerT& container, const ResultT& initval, const AccumulatorT& callback);



} // namespace Yuni

#include "fold.hxx"
