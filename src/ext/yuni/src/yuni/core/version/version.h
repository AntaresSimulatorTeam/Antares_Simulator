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
#include "../../yuni.h"
#include "../string.h"



namespace Yuni
{

	/*!
	** \brief Version number
	**
	** A version number is composed by two numbers (major and minor), plus a
	** revision number for fine-grained level.
	**
	** \ingroup Core
	*/
	class YUNI_DECL Version final
	{
	public:
		/*!
		** \brief Get the version of the Yuni Library
		*/
		static void InternalLib(Version& v);

	public:
		//! \name Constructor
		//@{
		//! Default constructor
		Version();

		/*!
		** \brief Constructor with a given version
		**
		** \param major The major version number
		*/
		explicit Version(uint major);

		/*!
		** \brief Constructor with a given version
		**
		** \param major The major version number
		** \param minor The minor version number
		*/
		Version(uint major, uint minor);

		/*!
		** \brief Constructor with a given version
		**
		** \param major The major version number
		** \param minor The minor version number
		** \param rev   The revision number
		*/
		Version(uint major, uint minor, uint rev);

		//! Copy constructor
		Version(const Version& c);
		//@}


		//! \name Conversions
		//@{
		/*!
		** \brief Get the version in an human-readable string
		*/
		String toString() const;
		//@}


		//! \name Initialization
		//@{
		/*!
		** \brief Assign a new version numner
		*/
		void assign(uint major, uint minor, uint r = 0);

		/*!
		** \brief Set the version to 0.0.0
		*/
		void clear();
		//@}


		//! \name Comparisons
		//@{
		/*!
		** \brief Get if the version is null
		*/
		bool null() const;

		/*!
		** \brief Check if this version is less than another one
		*/
		bool isLessThan(const Version& rhs) const;
		/*!
		** \brief Check if this version is equal to another one
		*/
		bool isEqualTo(const Version& rhs) const;
		/*!
		** \brief Check if this version is greater than another one
		*/
		bool isGreaterThan(const Version& rhs) const;
		//@}


		//! \name ostream
		//@{
		/*!
		** \brief Print the version to a ostream
		*/
		template<class S> void print(S& out) const;
		//@}


		//! \name Operators
		//@{
		//! The operator <
		bool operator <  (const Version& rhs) const;
		//! The operator <=
		bool operator <= (const Version& rhs) const;
		//! The operator >
		bool operator >  (const Version& rhs) const;
		//! The operator <=
		bool operator >= (const Version& rhs) const;

		//! The operator ==
		bool operator == (const Version& rhs) const;
		//! The operator !=
		bool operator != (const Version& rhs) const;

		//! The operator =
		Version& operator = (const Version& rhs);
		//@}


	public:
		//! The major version number
		uint hi;
		//! The minor version number
		uint lo;
		//! Revision
		uint revision;

	}; // class Version






} // namespace Yuni

#include "version.hxx"
