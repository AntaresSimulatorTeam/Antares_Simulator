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
#include "program.h"



namespace Yuni
{
namespace Process
{

	template<class MainLoopT>
	inline bool Execute(MainLoopT& mainloop, const AnyString& commandLine, uint timeout)
	{
		Program program;
		program.commandLine(commandLine);
		return (program.execute(mainloop, timeout)) ? (0 == program.wait()) : false;
	}




	template<class MainLoopT>
	inline bool Program::execute(MainLoopT& mainloop, uint timeout)
	{
		Bind<void (const Callback&)> dispatcher;
		dispatcher.bind<MainLoopT>(&mainloop, & MainLoopT::dispatch);
		return dispatchExecution(dispatcher, timeout);
	}


	inline Stream::Ptr  Program::stream() const
	{
		return pStream;
	}


	inline void Program::stream(Stream::Ptr newstream)
	{
		pStream = newstream;
	}





} // namespace Process
} // namespace Yuni
