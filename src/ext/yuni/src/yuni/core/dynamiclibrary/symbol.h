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
/*!
** Header for Yuni::DynamicLibrary::Symbol, a class for representing an exported
** symbol by a (shared) library
*/
#include "../../yuni.h"




namespace Yuni
{
namespace DynamicLibrary
{

	/*!
	** \brief Exported Symbol from a dynamic library
	** \ingroup DynamicLibs
	*/
	class YUNI_DECL Symbol final
	{
	public:
		//! Handle for a symbol
		typedef void* Handle;

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		Symbol();
		//! Constructor with a given handle
		Symbol(Handle p);
		//! Copy constructor
		Symbol(const Symbol& rhs);
		//@}


		//! \name Validity
		//@{
		//! Get if the symbol is invalid
		bool null() const;
		//! Get if the symbol is valid
		bool valid() const;
		//@}


		//! \name Operator
		//@{
		//! Copy operator
		Symbol& operator = (const Symbol& rhs);
		//! Copy operator
		Symbol& operator = (Symbol::Handle hndl);
		//@}


		//! \name Handle
		//@{
		/*!
		** \brief Get the handle of the symbol
		*/
		Handle ptr() const;
		//@}


	private:
		//! Handle
		Handle pPtr;

	}; // class Symbol





} // namespace DynamicLibrary
} // namespace Yuni

#include "symbol.hxx"

