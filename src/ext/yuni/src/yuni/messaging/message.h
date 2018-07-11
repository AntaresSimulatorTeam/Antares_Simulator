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
#ifndef __YUNI_MESSAGING_MESSAGE_H__
# define __YUNI_MESSAGING_MESSAGE_H__

# include "../yuni.h"
# include "../core/dictionary.h"


namespace Yuni
{
namespace Messaging
{

	class Message final
	{
	public:
		Message() :
			httpStatus(200)
		{}

	public:
		// returned message
		Clob body;
		// HTTP Error
		uint httpStatus;
		// parameters
		KeyValueStore params;
		// Name of the method invoked
		AnyString method;
		// Schema
		AnyString schema;

		//! Temporary string provided for convenient uses (to reduce memory reallocation)
		String key;

	}; // class Message




} // namespace Messaging
} // namespace Yuni

#endif // __YUNI_MESSAGING_MESSAGE_H__
