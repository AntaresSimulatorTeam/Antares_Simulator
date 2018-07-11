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
namespace Policy
{

/*!
** \brief Constness policies
** \ingroup Policies
*/
namespace Constness
{


	/*!
	** \brief Don't Propagate constness of pointed or referred object
	** \ingroup Policies
	*/
	template<typename T>
	struct DontPropagateConst
	{
		typedef T Type;
	};


	/*!
	** \brief Propagate constness of pointed or referred object
	** \ingroup Policies
	*/
	template<typename T>
	struct PropagateConst
	{
		typedef const T Type;
	};





} // namespace Constness
} // namespace Policy
} // namespace Yuni

