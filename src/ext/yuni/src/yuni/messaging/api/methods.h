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
#ifndef __YUNI_MESSAGING_API_METHODS_H__
# define __YUNI_MESSAGING_API_METHODS_H__

# include "../../yuni.h"
# include "fwd.h"
# include "method.h"


namespace Yuni
{
namespace Messaging
{
namespace API
{

	//! All methods
	class Methods final
	{
	public:
		/*!
		 ** \brief Add a new method
		 */
		Method& add(const AnyString& name);

		/*!
		** \brief Remove all methods
		*/
		void clear();

		//! All methods
		const Method::Hash& all() const;

		//! Reduce the memory consumption as much as possible
		void shrinkMemory();

	private:
		//! methods
		Method::Hash  pMethods;

	}; // class Methods





} // namespace API
} // namespace Messaging
} // namespace Yuni

# include "methods.hxx"

#endif // __YUNI_MESSAGING_API_METHODS_H__
