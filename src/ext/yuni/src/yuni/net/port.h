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
#ifndef __YUNI_NET_PORT_H__
# define __YUNI_NET_PORT_H__

# include "../yuni.h"


namespace Yuni
{
namespace Net
{

	/*!
	** \brief Socket Port number
	**
	** \see http://www.iana.org/assignments/port-numbers
	*/
	class Port
	{
	public:
		struct Range
		{
			//! Get if the port is well-known (0..1023)
			static bool IsWellKnown(const Port& port);
			//! Get if the port is registered (0..49151)
			static bool IsRegistered(const Port& port);
			//! Get if the port is dynamic and/or private (>= 49152)
			static bool IsDynamic(const Port& port);
		};

	public:
		//! \name Constructors
		//@{
		//! Default constructor
		Port();
		//! Constructor with a given value
		Port(uint rhs);
		//! Copy constructor
		Port(const Port& rhs);
		//! Constructor from a null pointer
		Port(const NullPtr&);
		//@}

		//! \name Value
		//@{
		//! Get the port number
		uint value() const;
		//! Get if the port is valid
		bool valid() const;

		//! Get if no port is allocated
		bool none() const;
		//@}

		//! \name Operators
		//@{
		//! Assignment
		Port& operator = (uint rhs);
		Port& operator = (const Port& rhs);
		Port& operator = (const Yuni::NullPtr&);
		//! Append
		Port& operator += (uint rhs);
		//! Sub
		Port& operator -= (uint rhs);
		//! Comparison
		bool operator == (uint rhs) const;
		bool operator == (const Port& rhs) const;
		bool operator != (uint rhs) const;
		bool operator != (const Port& rhs) const;
		bool operator ! () const;
		//! Inequality
		bool operator < (const Port& rhs) const;
		bool operator > (const Port& rhs) const;
		bool operator <= (const Port& rhs) const;
		bool operator >= (const Port& rhs) const;

		//! Cast uint
		operator uint () const;
		//@}

	private:
		//! Port value
		uint pValue;

	}; // class Port






} // namespace Net
} // namespace Yuni

# include "port.hxx"

#endif // __YUNI_NET_PORT_H__
