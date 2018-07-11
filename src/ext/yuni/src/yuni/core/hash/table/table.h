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
#include <map>


namespace Yuni
{
namespace Hash
{


	template<class KeyT, class ValueT, class ImplT = std::map<KeyT,ValueT> >
	class Index : protected ImplT
	{
	public:
		//! Type of the key
		typedef KeyT KeyType;
		//! Type for a value
		typedef ValueT ValueType;

		//! The real implementation
		typedef ImplT ImplementationType;

		//! Size
		typedef typename ImplementationType::size_type size_type;

	public:
		//! \name Constructors & Destructor
		//@{
		Table();
		//! Destructor
		~Table();
		//@}

		template<class K> iterator addOrUpdate(const K& key)
		{
			return ImplementationType::template addOrUpdate<K>(key);
		}

		template<class K, class V> iterator addOrUpdate(const K& key, const V& value)
		{
			return ImplementationType::template addOrUpdate<K,V>(key, value);
		}

		/*!
		** \brief Remove all items
		*/
		void clear()
		{
			ImplementationType::clear();
		}

		/*!
		** \brief Clear all deleted items (if any)
		*/
		void purge()
		{
			ImplementationType::purge();
		}

		template<class K>
		bool exists(const K& key) const
		{
			return ImplementationType::exists(key);
		}

		template<class K> bool remove(const K& key)
		{
			return false;
		}

		bool empty() const
		{
			return ImplementationType::empty();
		}

		size_type size() const
		{
			return ImplementationType::size();
		}

		template<class K> ValueType& operator [] (const K& key)
		{
		}

	}; // class Table<>





} // namespace Hash
} // namespace Yuni
