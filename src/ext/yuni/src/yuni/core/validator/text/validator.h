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
#ifndef __YUNI_CORE_VALIDATOR_VALIDATOR_H__
# define __YUNI_CORE_VALIDATOR_VALIDATOR_H__

# include "../../../yuni.h"
# include "../../string.h"
# include "../validator.h"



namespace Yuni
{
namespace Validator
{
namespace Text
{



	template<class D>
	class IValidatorTmpl
	{
	public:
		/*!
		** \brief Perform a validation on any type of string
		**
		** \param u An arbitrary string
		** \return True if valid, false otherwise
		*/
		template<class U> bool validate(const U& u) const
		{
			return static_cast<const D*>(this)->validate(u);
		}

		/*!
		** \see validate()
		*/
		template<class U> bool operator () (const U& u) const
		{
			return static_cast<const D*>(this)->validate(u);
		}

	}; // class IValidatorTmpl





} // namespace Text
} // namespace Validator
} // namespace Yuni

# include "default.h"

#endif // __YUNI_CORE_VALIDATOR_VALIDATOR_H__
