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
#include "../yuni.h"
#include "string.h"
#include <map>
#include <set>
#ifdef YUNI_HAS_STL_UNORDERED_MAP
# include <unordered_map>
#endif
#ifdef YUNI_HAS_STL_UNORDERED_SET
# include <unordered_set>
#endif



namespace Yuni
{

	/*!
	** \brief Class helper for defining ordered/unordered map
	** \ingroup Core
	*/
	template<class KeyT, class ValueT>
	class Dictionary final
	{
	public:
		# ifndef YUNI_HAS_STL_UNORDERED_MAP
		//! An unordered dictionary table
		typedef std::map<KeyT, ValueT> Hash;
		//! An unordered dictionary table
		typedef std::map<KeyT, ValueT> Unordered;
		# else
		//! An unordered dictionary table
		typedef std::unordered_map<KeyT, ValueT> Hash;
		//! An unordered dictionary table
		typedef std::unordered_map<KeyT, ValueT> Unordered;
		# endif

		//! An ordered dictionary table
		typedef std::map<KeyT, ValueT> Map;
		//! An ordered dictionary table
		typedef std::map<KeyT, ValueT> Ordered;

	}; // struct Dictionary



	/*!
	** \brief Class helper for defining ordered/unordered set
	** \ingroup Core
	*/
	template<class KeyT>
	class Set final
	{
	public:
		# ifndef YUNI_HAS_STL_UNORDERED_MAP
		//! An unordered set table
		typedef std::set<KeyT> Hash;
		//! An unordered set table
		typedef std::set<KeyT> Unordered;
		# else
		//! An unordered set table
		typedef std::unordered_set<KeyT> Hash;
		//! An unordered set table
		typedef std::unordered_set<KeyT> Unordered;
		# endif

		//! An ordered dictionary table
		typedef std::set<KeyT> Ordered;

	}; // struct Set



	/*!
	** \brief Convenient typedef for a key/value store
	** \ingroup Core
	*/
	typedef Dictionary<String, String>::Hash  KeyValueStore;





} // namespace Yuni

