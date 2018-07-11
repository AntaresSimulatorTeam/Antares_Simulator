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
#ifndef __YUNI_MESSAGING_THREAD_CONTEXT_H__
# define __YUNI_MESSAGING_THREAD_CONTEXT_H__

# include "../yuni.h"
# include "fwd.h"
# include "../core/dictionary.h"


namespace Yuni
{
namespace Messaging
{

	/*!
	** \brief Schema for API methods
	**
	** A schema is some kink of namespace, where methods belong to
	*/
	class YUNI_DECL Context final
	{
	public:
		class HttpStatusCode final
		{
		public:
			enum
			{
				max2xx = 204,
				max4xx = 404,
				max5xx = 500,
			};
			ShortString32 header2xx[5];
			ShortString32 header4xx[5];
			ShortString32 header5xx[1];
			ShortString64 response4xx[5];
			ShortString64 response5xx[1];
		};

	public:
		/*!
		** \brief Default constructor
		**
		** \param service Reference to the original service instance
		*/
		Context(Service& service);

		//! Clear all internal variables
		void clear();

		//! Reduce memory usage by internal variables if necessary,
		// usefull for some memory-hungry apps
		void autoshrink();


	public:
		//! Parameters
		KeyValueStore params;
		//! User data pointer
		void* userdata;

		//! String preallocated for various usage (text with small size, < 6Ko)
		// to reduce memory usage, the internal memory of this variable will automatically
		// be wipe out if using more than 6KiB
		String text;
		//! Clob for various uses (text from medium to large size)
		// to reduce memory usage, the internal memory of this variable will automatically
		// be wipe out if using more than 5MiB
		Clob clob;

		//! Name of the method invoked
		AnyString method;
		//! Schema of the method invoked
		AnyString schema;

		//! Http status
		uint httpStatus;

		//! Remote port of the current connection
		uint remotePort;

		//! Buffer reused by transports for generating a response
		// However, it can be safely used by the method invoked
		Clob buffer;

		//! Service
		Service& service;

		//! Pre-prepared header responses
		const HttpStatusCode httpStatusCode;

	private:
		template<uint MaxSize, class StringT> void AutoShrink(StringT& variable);

	}; // class ThreadContext





} // namespace Messaging
} // namespace Yuni

# include "threadcontext.hxx"

#endif // __YUNI_MESSAGING_THREAD_CONTEXT_H__
