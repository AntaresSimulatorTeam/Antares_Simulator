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
#ifndef __ANTARES_LIBS_STUDY_SETS_H__
# define __ANTARES_LIBS_STUDY_SETS_H__

# include <yuni/yuni.h>
# include <yuni/core/string.h>
# include <map>
# include <set>
# include <cassert>
# include "../inifile.h"
# include "../logs.h"


namespace Antares
{
namespace Data
{


	template<class T>
	class Sets final
	{
	public:
		//! Type
		typedef T Type;

		//
		typedef Yuni::CString<128, false> IDType;
		//! Value
		typedef Yuni::SmartPtr<T, Yuni::Policy::Ownership::ReferenceCounted> TypePtr;

		//! Map of Item
		typedef std::map<IDType, TypePtr>  MapType;
		//! Standard iterators from the STL
		typedef typename MapType::iterator iterator;
		//! Standard iterators from the STL (const)
		typedef typename MapType::const_iterator const_iterator;

		enum RuleType
		{
			ruleNone = 0,
			ruleAdd,
			ruleRemove,
			ruleFilter,
			ruleMax,
		};
		//! Definition of a single rule
		typedef std::pair<RuleType, Yuni::String::Ptr>  Rule;
		//! Rule Set
		typedef std::vector<Rule> RuleSet;


		class Options final
		{
		public:
			Options() :
				output(true), resultSize(0)
			{}
			Options(const Options& rhs) :
				caption(rhs.caption),
				comments(rhs.comments),
				rules(rhs.rules),
				output(rhs.output),
				resultSize(rhs.resultSize)
			{}

			void reset(const IDType& id)
			{
				caption = id;
				comments.clear();
				rules.clear();
				output = false;
				resultSize = 0;
			}

			Options& operator = (const Options& rhs)
			{
				caption    = rhs.caption;
				comments   = rhs.comments;
				rules      = rhs.rules;
				output     = rhs.output;
				resultSize = rhs.resultSize;
				return *this;
			}

		public:
			//! Caption
			IDType caption;
			//! Comments
			Yuni::String comments;
			//! Rules to build the group
			RuleSet rules;
			//! Enable/Disable the results in the output
			bool output;
			//! The number of items in the result set
			uint resultSize;

		}; // class Options

		typedef std::map<IDType, Options> MapOptions;


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Sets();
		/*!
		** \brief Copy constructor
		*/
		Sets(const Sets& rhs);
		//! Destructor
		~Sets();
		//@}


		//! \name Iterators
		//@{
		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
		//@}


		/*!
		** \brief Clear all groups
		*/
		void clear();

		/*!
		**
		*/
		TypePtr add(const IDType& name)
		{
			TypePtr p = new T();
			pMap[name] = p;
			pOptions[name].reset(name);
			return p;
		}

		/*!
		**
		*/
		TypePtr add(const IDType& name, const TypePtr& data)
		{
			pMap[name] = data;
			pOptions[name].reset(name);
			return data;
		}

		/*!
		**
		*/
		TypePtr add(const IDType& name, const TypePtr& data, Options& opts)
		{
			pMap[name] = data;
			pOptions[name] = opts;
			return data;
		}


		bool invalidate(bool /*reload*/) const
		{
			pModified = true;
			return true;
		}

		void markAsModified() const
		{
			pModified = true;
		}


		uint size() const;

		void rebuildIndexes();

		/*!
		** \brief Get if the results for a given group should be written to the output
		*/
		template<class StringT> bool hasOutput(const StringT& s) const;

		/*!
		** \brief Get if the results for a given group should be written to the output
		*/
		bool hasOutput(const uint index) const;

		/*!
		** \brief Get the size of a result set
		*/
		template<class StringT> uint resultSize(const StringT& s) const;

		/*!
		** \brief Get the size of a result set
		*/
		uint resultSize(const uint index) const;

		template<class L> void dumpToLogs(L& log) const;

		/*!
		** \brief Load a rule set from an INI File
		*/
		template<class StringT> bool loadFromFile(const StringT& filename);

		template<class StringT> bool saveToFile(const StringT& filename) const;

		/*!
		** \brief Create default groups for set of areas
		*/
		void defaultForAreas();

		/*!
		** \brief Rebuild the lists of a group from the rules
		*/
		template<class HandlerT>
		void rebuildFromRules(const IDType& id, HandlerT& handler);

		/*!
		** \brief Rebuild the lists of all group from the rules
		*/
		template<class HandlerT>
		void rebuildAllFromRules(HandlerT& handler);


		const IDType& nameByIndex(const uint i) const
		{
			assert(i < pMap.size() && "Sets: operator[] index out of bounds");
			return pNameByIndex[i];
		}

		template<class StringT>
		IDType caption(const StringT& s) const;

		IDType caption(const uint i) const;


		T& operator [] (uint i);
		const T& operator [] (uint i) const;

	private:
		//! All groups
		MapType pMap;
		MapOptions pOptions;
		//!
		TypePtr* pByIndex;
		IDType* pNameByIndex;
		mutable bool pModified;

	}; // class Sets






} // namespace Data
} // namespace Antares

# include "sets.hxx"

#endif // __ANTARES_LIBS_STUDY_SETS_H__
