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
#include "../../../yuni.h"
#include "../../smartptr.h"




namespace Yuni
{
namespace Process
{

	/*!
	** \brief Process Stream
	**
	** The lifetime of a process stream is guarantee to be at least
	** the whole execution of the underlying process
	*/
	class Stream
	{
	public:
		typedef SmartPtr<Stream> Ptr;

	public:
		Stream() {}
		virtual ~Stream() {}

		//! Some data from the standard output are ready
		virtual void onRead(const AnyString& /*buffer*/) {}

		//! Some data from the error output are ready
		virtual void onErrorRead(const AnyString& /*buffer*/) {}

		//! The execution has finished
		virtual void onStop(bool /*killed*/, int /*exitstatus*/, sint64 /*duration*/) {}
	};





	class CaptureOutput : public Process::Stream
	{
	public:
		typedef SmartPtr<CaptureOutput>  Ptr;

	public:
		CaptureOutput() {}
		virtual ~CaptureOutput() {}

		//! Some data from the standard output are ready
		virtual void onRead(const AnyString& buffer) override
		{
			cout += buffer;
		}

		//! Some data from the error output are ready
		virtual void onErrorRead(const AnyString& buffer) override
		{
			cerr += buffer;
		}


	public:
		Clob cout;
		Clob cerr;

	}; // class CaptureOutput





} // namespace Process
} // namespace Yuni
