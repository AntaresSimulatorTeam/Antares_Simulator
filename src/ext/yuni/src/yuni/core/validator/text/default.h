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
#ifndef __YUNI_CORE_VALIDATOR_DEFAULT_H__
# define __YUNI_CORE_VALIDATOR_DEFAULT_H__

# include "validator.h"
# include "default.private.h"


namespace Yuni
{
namespace Validator
{
namespace Text
{


	/*!
	** \brief Allow all strings / char
	*/
	class AllowAll : public IValidatorTmpl<AllowAll>
	{
	public:
		template<class U> bool validate(const U&) const
		{
			return true; // allow all
		}

	}; // class AllowAll


	/*!
	** \brief Deny all strings / char
	*/
	class DenyAll : public IValidatorTmpl<DenyAll>
	{
	public:
		template<class U> bool validate(const U&) const
		{
			return false; // deny all
		}

	}; // class DenyAll




	/*!
	** \brief Allow/Deny all strings/char by default, with a list of exceptions
	**
	** \code
	** Validator::Text::ExceptionList<Validator::allowByDefault> validator;
	** validator.exception("january");
	** validator.exception("december");
	**
	** std::cout << validator("february") << std::endl; // True
	** std::cout << validator("december") << std::endl; // False
	** \endcode
	**
	** A more convenient typedef is AllowByDefault and DenyByDefauly.
	** \see typedef AllowByDefault
	** \see typedef DenyByDefauly
	*/
	template<Yuni::Validator::DefaultPolicy DefaultPolicy>
	class ExceptionList final : public IValidatorTmpl<ExceptionList<DefaultPolicy> >
	{
	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default Constructor
		*/
		ExceptionList();
		/*!
		** \brief Copy constructor
		*/
		ExceptionList(const ExceptionList& rhs);
		/*!
		** \brief Copy constructor
		*/
		template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
		ExceptionList(const ExceptionList<OtherDefaultPolicy>& rhs);
		/*!
		** \brief Constructor with a default exception list
		*/
		ExceptionList(const String::Vector& rhs);
		/*!
		** \brief Constructor with a default exception list
		*/
		ExceptionList(const String::List& rhs);
		//@}


		//! \name String validation
		//@{
		/*!
		** \brief Validate (or not) a string
		*/
		template<class U> bool validate(const U& s) const;
		//@}


		//! \name Add an exception in the list
		//@{
		/*!
		** \brief Add an exception
		**
		** \param text The exception (arbitrary string)
		*/
		template<class U> void exception(const U& text);

		/*!
		** \brief Append an arbitrary string
		** \param text A arbitrary string
		*/
		ExceptionList& operator += (const AnyString& text);

		template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
		ExceptionList& operator += (const ExceptionList<OtherDefaultPolicy>& rhs);

		//! Operator += on a std::vector
		ExceptionList& operator += (const String::Vector& rhs);

		//! Operator += on a std::list
		ExceptionList& operator += (const String::List& rhs);

		//! Operator << on an arbitrary string
		ExceptionList& operator << (const AnyString& rhs);

		//! Operator << on another exception list
		template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
		ExceptionList& operator << (const ExceptionList<OtherDefaultPolicy>& rhs);

		//! Operator << on a std::vector
		ExceptionList& operator << (const String::Vector& rhs);

		//! Operator << on a std::list
		ExceptionList& operator << (const String::List& rhs);
		//@}


		//! \name Reset the exception list
		//@{
		//! Operator =
		template<Yuni::Validator::DefaultPolicy OtherDefaultPolicy>
		ExceptionList& operator = (const ExceptionList<OtherDefaultPolicy>& rhs);
		//! Operator =
		ExceptionList& operator = (const String::Vector& rhs);
		//! Operator =
		ExceptionList& operator = (const String::List& rhs);
		//@}

	private:
		//! List of exceptions
		String::Vector pExceptionList;

	}; // class ExceptionList





	/*!
	** \brief Allow all strings/char by default, with an exception list
	*/
	typedef ExceptionList<Validator::allowByDefault>  AllowByDefault;

	/*!
	** \brief Deny all strings/char by default, with an exception list
	*/
	typedef ExceptionList<Validator::denyByDefault>   DenyByDefault;




} // namespace Text
} // namespace Validator
} // namespace Yuni

# include "default.hxx"

#endif // __YUNI_CORE_VALIDATOR_DEFAULT_H__
