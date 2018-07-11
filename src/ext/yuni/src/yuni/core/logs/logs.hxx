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



namespace Yuni
{
namespace Logs
{


	template<class Handlers, class Decorators, template<class> class TP>
	inline Logger<Handlers,Decorators,TP>::Logger() :
		verbosityLevel(Logger<Handlers,Decorators,TP>::defaultVerbosityLevel)
	{}


	template<class Handlers, class Decorators, template<class> class TP>
	inline Logger<Handlers,Decorators,TP>::~Logger()
	{}


	template<class Handlers, class Decorators, template<class> class TP>
	template<class VerbosityType>
	inline void
	Logger<Handlers,Decorators,TP>::dispatchMessageToHandlers(const AnyString& message) const
	{
		// Locking the operation, according to the threading policy
		typename ThreadingPolicy::MutexLocker locker(*this);

		// Filtering the verbosity level
		// 'verbosityLevel' is a public variable
		if (VerbosityType::level <= verbosityLevel)
		{
			Logger<Handlers,Decorators,TP>&	self = const_cast<Logger<Handlers,Decorators,TP>&>(*this);
			// Ask to all handlers to internalDecoratorWriteWL the message
			Handlers::template
				internalDecoratorWriteWL<LoggerType,VerbosityType>(self, message);
		}
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::NoticeBuffer
	Logger<Handlers,Decorators,TP>::notice() const
	{
		return NoticeBuffer(*this);
	}

	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::InfoBuffer
	Logger<Handlers,Decorators,TP>::info() const
	{
		return InfoBuffer(*this);
	}

	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::CompatibilityBuffer
	Logger<Handlers,Decorators,TP>::compatibility() const
	{
		return CompatibilityBuffer(*this);
	}



	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::CheckpointBuffer
	Logger<Handlers,Decorators,TP>::checkpoint() const
	{
		return typename Logger<Handlers,Decorators,TP>::CheckpointBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::WarningBuffer
	Logger<Handlers,Decorators,TP>::warning() const
	{
		return WarningBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::ErrorBuffer
	Logger<Handlers,Decorators,TP>::error() const
	{
		return ErrorBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::ProgressBuffer
	Logger<Handlers,Decorators,TP>::progress() const
	{
		return ProgressBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::FatalBuffer
	Logger<Handlers,Decorators,TP>::fatal() const
	{
		return FatalBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	inline typename Logger<Handlers,Decorators,TP>::DebugBuffer
	Logger<Handlers,Decorators,TP>::debug() const
	{
		return DebugBuffer(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::NoticeBuffer
	Logger<Handlers,Decorators,TP>::notice(const U& u) const
	{
		return NoticeBuffer(*this, u);
	}

	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::InfoBuffer
	Logger<Handlers,Decorators,TP>::info(const U& u) const
	{
		return InfoBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::CheckpointBuffer
	Logger<Handlers,Decorators,TP>::checkpoint(const U& u) const
	{
		return CheckpointBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::WarningBuffer
	Logger<Handlers,Decorators,TP>::warning(const U& u) const
	{
		return WarningBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::ErrorBuffer
	Logger<Handlers,Decorators,TP>::error(const U& u) const
	{
		return ErrorBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::ProgressBuffer
	Logger<Handlers,Decorators,TP>::progress(const U& u) const
	{
		return ProgressBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::CompatibilityBuffer
	Logger<Handlers,Decorators,TP>::compatibility(const U& u) const
	{
		return CompatibilityBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::FatalBuffer
	Logger<Handlers,Decorators,TP>::fatal(const U& u) const
	{
		return FatalBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::DebugBuffer
	Logger<Handlers,Decorators,TP>::debug(const U& u) const
	{
		return DebugBuffer(*this, u);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<class C>
	inline Private::LogImpl::Buffer<Logger<Handlers,Decorators,TP>, C, C::enabled>
	Logger<Handlers,Decorators,TP>::custom() const
	{
		return Private::LogImpl::Buffer<LoggerType, C, C::enabled>(*this);
	}


	template<class Handlers, class Decorators, template<class> class TP>
	template<typename U>
	inline typename Logger<Handlers,Decorators,TP>::UnknownBuffer
	Logger<Handlers,Decorators,TP>::operator << (const U& u) const
	{
		return UnknownBuffer(*this, u);
	}





} // namespace Logs
} // namespace Yuni
