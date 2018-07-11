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
#include "../string.h"



namespace Yuni
{
namespace Private
{
namespace Uri
{


	/*!
	** \brief Carrier for informations about an URI
	*/
	class YUNI_DECL Informations final
	{
	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Informations();
		//! Copy constructor
		Informations(const Informations& rhs);
		//@}

		/*!
		** \brief Assign values from another struct `Informations`
		** \param rhs Another instance
		*/
		void assign(const Informations& rhs);

		/*!
		** \brief Clear all data
		*/
		void clear();

		/*!
		** \brief Convert into a mere string
		*/
		String toString() const;

		/*!
		** \brief Get if equals to another instance
		*/
		bool isEqualsTo(const Informations& rhs) const;

		/*!
		** \brief Print the uri to a stream
		*/
		void print(std::ostream& out) const;

	public:
		//! Scheme
		String scheme;
		//! User
		String user;
		//! password
		String password;
		//! server
		String server;
		//! port
		sint32 port;
		//! path
		String path;
		//! query
		String query;
		//! fragment
		String fragment;
		//! Were the informations of the URI valid during the last build
		bool isValid;

	}; // class Informations




} // namespace Uri
} // namespace Private
} // namespace Yuni
